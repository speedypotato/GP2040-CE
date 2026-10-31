@echo off
rem GP2040-CE: bind a Mayflash S5 controller to this PC for wireless use.
rem Double-click with the controller plugged in over USB in PS5 mode.
rem Extracts the PowerShell script below to %TEMP% and runs it (offline).
setlocal
set "S5SELF=%~f0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "$s=[IO.File]::ReadAllText($env:S5SELF); $i=$s.IndexOf('#'+'PSSTART'); $p=Join-Path $env:TEMP 's5-bind-windows.ps1'; [IO.File]::WriteAllText($p,$s.Substring($i)); & $p %*"
exit /b
#PSSTART
<#
.SYNOPSIS
  Proof of concept: bind a GP2040-CE + Mayflash S5 controller (USB 054c:0ce6)
  to this Windows PC so it reconnects over Bluetooth after unplugging.
  Offline. Needs administrator rights (the key store is SYSTEM-only).

  What it does (the same thing macOS does on plug-in):
    1. Reads the S5's Bluetooth address over USB (feature report 0x09).
    2. Reads this PC's Bluetooth radio address.
    3. Generates a random 16-byte link key.
    4. Stores the key in Windows' Bluetooth key store as SYSTEM:
       HKLM\SYSTEM\CurrentControlSet\Services\BTHPORT\Parameters\Keys\<radio>\<device>
    5. Restarts the Bluetooth radio so Windows loads the key.
    6. Writes this PC's address + the key to the S5 (0x0A), then 0x08 = 0x01.
    7. Asks Windows to enable the HID service for the S5 (connect + install driver).

.PARAMETER ReverseKey
  Write the key to the S5 in reversed byte order. Use if binding succeeds but
  the controller never connects (key byte order between 0x0A and Windows is unverified).
.PARAMETER Unbind
  Remove this PC's pairing for the controller and exit.
#>
[CmdletBinding()]
param(
    [switch]$ReverseKey,
    [switch]$Unbind,
    [switch]$NoConnect
)
$ErrorActionPreference = 'Stop'

# --- self-elevate -----------------------------------------------------------
$principal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    $argList = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', "`"$PSCommandPath`"")
    if ($ReverseKey) { $argList += '-ReverseKey' }
    if ($Unbind)     { $argList += '-Unbind' }
    if ($NoConnect)  { $argList += '-NoConnect' }
    Start-Process -FilePath 'powershell.exe' -Verb RunAs -ArgumentList $argList
    exit
}

# --- native helpers (C# 5, compiled locally by Windows PowerShell) ----------
Add-Type -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

public static class S5Native {
    [StructLayout(LayoutKind.Sequential)]
    public struct SP_DEVICE_INTERFACE_DATA { public int cbSize; public Guid InterfaceClassGuid; public int Flags; public IntPtr Reserved; }

    [StructLayout(LayoutKind.Sequential)]
    public struct HIDP_CAPS {
        public ushort Usage, UsagePage, InputReportByteLength, OutputReportByteLength, FeatureReportByteLength;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 17)] public ushort[] Reserved;
        public ushort NumberLinkCollectionNodes, NumberInputButtonCaps, NumberInputValueCaps, NumberInputDataIndices,
                      NumberOutputButtonCaps, NumberOutputValueCaps, NumberOutputDataIndices,
                      NumberFeatureButtonCaps, NumberFeatureValueCaps, NumberFeatureDataIndices;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BLUETOOTH_FIND_RADIO_PARAMS { public int dwSize; }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct BLUETOOTH_RADIO_INFO {
        public int dwSize; public ulong address;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 248)] public string szName;
        public uint ulClassofDevice; public ushort lmpSubversion; public ushort manufacturer;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct SYSTEMTIME { public ushort y, mo, dow, d, h, mi, s, ms; }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct BLUETOOTH_DEVICE_INFO {
        public int dwSize; public ulong Address; public uint ulClassofDevice;
        public bool fConnected; public bool fRemembered; public bool fAuthenticated;
        public SYSTEMTIME stLastSeen; public SYSTEMTIME stLastUsed;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 248)] public string szName;
    }

    [DllImport("hid.dll")] static extern void HidD_GetHidGuid(out Guid g);
    [DllImport("hid.dll", SetLastError = true)] static extern bool HidD_GetFeature(SafeFileHandle h, byte[] b, int n);
    [DllImport("hid.dll", SetLastError = true)] static extern bool HidD_SetFeature(SafeFileHandle h, byte[] b, int n);
    [DllImport("hid.dll")] static extern bool HidD_GetPreparsedData(SafeFileHandle h, out IntPtr pp);
    [DllImport("hid.dll")] static extern bool HidD_FreePreparsedData(IntPtr pp);
    [DllImport("hid.dll")] static extern int HidP_GetCaps(IntPtr pp, out HIDP_CAPS caps);
    [DllImport("setupapi.dll", CharSet = CharSet.Unicode)] static extern IntPtr SetupDiGetClassDevs(ref Guid g, IntPtr e, IntPtr w, int f);
    [DllImport("setupapi.dll")] static extern bool SetupDiEnumDeviceInterfaces(IntPtr s, IntPtr di, ref Guid g, int i, ref SP_DEVICE_INTERFACE_DATA d);
    [DllImport("setupapi.dll", CharSet = CharSet.Unicode)] static extern bool SetupDiGetDeviceInterfaceDetail(IntPtr s, ref SP_DEVICE_INTERFACE_DATA d, IntPtr det, int size, out int req, IntPtr di);
    [DllImport("setupapi.dll")] static extern bool SetupDiDestroyDeviceInfoList(IntPtr s);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern SafeFileHandle CreateFile(string n, uint a, uint s, IntPtr sa, uint disp, uint f, IntPtr t);

    [DllImport("BluetoothApis.dll", SetLastError = true)] static extern IntPtr BluetoothFindFirstRadio(ref BLUETOOTH_FIND_RADIO_PARAMS p, out IntPtr hRadio);
    [DllImport("BluetoothApis.dll")] static extern bool BluetoothFindRadioClose(IntPtr f);
    [DllImport("BluetoothApis.dll")] static extern uint BluetoothGetRadioInfo(IntPtr r, ref BLUETOOTH_RADIO_INFO i);
    [DllImport("BluetoothApis.dll")] static extern uint BluetoothGetDeviceInfo(IntPtr r, ref BLUETOOTH_DEVICE_INFO i);
    [DllImport("BluetoothApis.dll")] static extern uint BluetoothSetServiceState(IntPtr r, ref BLUETOOTH_DEVICE_INFO i, ref Guid svc, uint flags);
    [DllImport("BluetoothApis.dll")] public static extern uint BluetoothRemoveDevice(ref ulong addr);
    [DllImport("kernel32.dll")] public static extern bool CloseHandle(IntPtr h);

    public static List<string> HidPaths() {
        var list = new List<string>();
        Guid g; HidD_GetHidGuid(out g);
        IntPtr set = SetupDiGetClassDevs(ref g, IntPtr.Zero, IntPtr.Zero, 0x12); // PRESENT | DEVICEINTERFACE
        try {
            for (int i = 0; ; i++) {
                var d = new SP_DEVICE_INTERFACE_DATA(); d.cbSize = Marshal.SizeOf(d);
                if (!SetupDiEnumDeviceInterfaces(set, IntPtr.Zero, ref g, i, ref d)) break;
                int req; SetupDiGetDeviceInterfaceDetail(set, ref d, IntPtr.Zero, 0, out req, IntPtr.Zero);
                IntPtr buf = Marshal.AllocHGlobal(req);
                try {
                    Marshal.WriteInt32(buf, IntPtr.Size == 8 ? 8 : 6);
                    if (SetupDiGetDeviceInterfaceDetail(set, ref d, buf, req, out req, IntPtr.Zero))
                        list.Add(Marshal.PtrToStringUni(new IntPtr(buf.ToInt64() + 4)));
                } finally { Marshal.FreeHGlobal(buf); }
            }
        } finally { SetupDiDestroyDeviceInfoList(set); }
        return list;
    }

    public static SafeFileHandle Open(string path) {
        var h = CreateFile(path, 0xC0000000, 3, IntPtr.Zero, 3, 0, IntPtr.Zero); // RW, share RW
        if (h.IsInvalid) h = CreateFile(path, 0, 3, IntPtr.Zero, 3, 0, IntPtr.Zero); // feature reports work without RW
        if (h.IsInvalid) throw new Exception("CreateFile failed: " + Marshal.GetLastWin32Error());
        return h;
    }

    public static int FeatureLength(SafeFileHandle h) {
        IntPtr pp; if (!HidD_GetPreparsedData(h, out pp)) throw new Exception("GetPreparsedData failed");
        try { HIDP_CAPS c; HidP_GetCaps(pp, out c); return c.FeatureReportByteLength; }
        finally { HidD_FreePreparsedData(pp); }
    }

    public static byte[] GetFeature(SafeFileHandle h, byte id) {
        var b = new byte[FeatureLength(h)]; b[0] = id;
        if (!HidD_GetFeature(h, b, b.Length)) throw new Exception("GetFeature 0x" + id.ToString("X2") + " failed: " + Marshal.GetLastWin32Error());
        return b;
    }

    public static void SetFeature(SafeFileHandle h, byte[] payload) {
        var b = new byte[FeatureLength(h)]; Array.Copy(payload, b, Math.Min(payload.Length, b.Length));
        if (!HidD_SetFeature(h, b, b.Length)) throw new Exception("SetFeature 0x" + payload[0].ToString("X2") + " failed: " + Marshal.GetLastWin32Error());
    }

    // Returns the open radio handle; caller closes it.
    public static IntPtr Radio(out ulong address) {
        var p = new BLUETOOTH_FIND_RADIO_PARAMS(); p.dwSize = Marshal.SizeOf(p);
        IntPtr hRadio; IntPtr f = BluetoothFindFirstRadio(ref p, out hRadio);
        if (f == IntPtr.Zero) throw new Exception("No Bluetooth radio found (is Bluetooth on?)");
        BluetoothFindRadioClose(f);
        var info = new BLUETOOTH_RADIO_INFO(); info.dwSize = Marshal.SizeOf(info);
        uint r = BluetoothGetRadioInfo(hRadio, ref info);
        if (r != 0) throw new Exception("BluetoothGetRadioInfo failed: " + r);
        address = info.address;
        return hRadio;
    }

    public static uint EnableHid(IntPtr hRadio, ulong device) {
        var i = new BLUETOOTH_DEVICE_INFO(); i.dwSize = Marshal.SizeOf(i); i.Address = device;
        BluetoothGetDeviceInfo(hRadio, ref i); // fills name/CoD if Windows already knows it; fine if not
        i.Address = device;
        Guid hid = new Guid("00001124-0000-1000-8000-00805f9b34fb");
        return BluetoothSetServiceState(hRadio, ref i, ref hid, 1); // BLUETOOTH_SERVICE_ENABLE
    }
}
'@

function Format-Addr([UInt64]$a) { ('{0:X12}' -f $a) -replace '(..)(?!$)', '$1:' }

# 6 little-endian address bytes -> UInt64 (Windows BLUETOOTH_ADDRESS layout)
function ConvertFrom-LEAddr([byte[]]$b) { $buf = New-Object byte[] 8; [Array]::Copy($b, 0, $buf, 0, 6); [BitConverter]::ToUInt64($buf, 0) }

function Invoke-AsSystem([string]$Body) {
    # Only SYSTEM may write the BTHPORT key store, so run a one-shot scheduled task.
    $dir = Join-Path $env:ProgramData 'S5Bind'
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
    $ps1 = Join-Path $dir 'stage.ps1'; $out = Join-Path $dir 'stage.out'
    Remove-Item $out -ErrorAction SilentlyContinue
    $wrapped = "try {`n$Body`n'OK' | Out-File -Encoding ascii '$out' } catch { ('ERR ' + `$_) | Out-File -Encoding ascii '$out' }"
    Set-Content -Path $ps1 -Value $wrapped -Encoding UTF8
    $name = 'S5BindStage'
    $action = New-ScheduledTaskAction -Execute 'powershell.exe' -Argument "-NoProfile -ExecutionPolicy Bypass -File `"$ps1`""
    Register-ScheduledTask -TaskName $name -Action $action -User 'SYSTEM' -RunLevel Highest -Force | Out-Null
    try {
        Start-ScheduledTask -TaskName $name
        for ($i = 0; $i -lt 40 -and -not (Test-Path $out); $i++) { Start-Sleep -Milliseconds 500 }
        if (-not (Test-Path $out)) { throw 'SYSTEM stage timed out' }
        $res = (Get-Content $out -Raw).Trim()
        if ($res -ne 'OK') { throw "SYSTEM stage failed: $res" }
    } finally {
        Unregister-ScheduledTask -TaskName $name -Confirm:$false -ErrorAction SilentlyContinue
        Remove-Item $ps1, $out -ErrorAction SilentlyContinue
    }
}

function Restart-BluetoothRadio {
    $radios = Get-PnpDevice -Class Bluetooth -PresentOnly |
        Where-Object { $_.InstanceId -like 'USB\*' -or $_.InstanceId -like 'PCI\*' }
    if (-not $radios) { Write-Warning 'Could not find the radio device to restart; reboot if the controller does not connect.' }
    foreach ($r in $radios) {
        Write-Host "  Restarting radio: $($r.FriendlyName)"
        Disable-PnpDevice -InstanceId $r.InstanceId -Confirm:$false
        Start-Sleep -Seconds 2
        Enable-PnpDevice -InstanceId $r.InstanceId -Confirm:$false
    }
    # Disabling/enabling the radio is enough for Windows to load the new key
    # (confirmed on a BCM20702); bthserv is not restarted.
    Start-Sleep -Seconds 5
}

try {
    # 1. controller over USB (Bluetooth instances have {00001124...} in the path)
    $path = [S5Native]::HidPaths() | Where-Object { $_ -match 'vid_054c&pid_0ce6' -and $_ -notmatch '00001124' } | Select-Object -First 1
    if (-not $path) { throw 'Controller not found over USB (054c:0ce6). Plug it in with a data cable.' }
    $h = [S5Native]::Open($path)
    $pi = [S5Native]::GetFeature($h, 0x09)
    $devBytes = $pi[1..6]                                  # little-endian
    [UInt64]$dev = ConvertFrom-LEAddr ([byte[]]$devBytes)
    Write-Host "Controller (S5) Bluetooth address: $(Format-Addr $dev)"

    # 2. this PC's radio
    [UInt64]$radio = 0
    $hRadio = [S5Native]::Radio([ref]$radio)
    [S5Native]::CloseHandle($hRadio) | Out-Null
    Write-Host "This PC's Bluetooth address:       $(Format-Addr $radio)"

    $radioKey = '{0:x12}' -f $radio
    $devName  = '{0:x12}' -f $dev
    $regPath  = "HKLM:\SYSTEM\CurrentControlSet\Services\BTHPORT\Parameters\Keys\$radioKey"

    # Remove any previous Windows pairing for this controller.
    [UInt64]$tmp = $dev; [S5Native]::BluetoothRemoveDevice([ref]$tmp) | Out-Null

    if ($Unbind) {
        Invoke-AsSystem "if (Test-Path '$regPath') { Remove-ItemProperty -Path '$regPath' -Name '$devName' -ErrorAction SilentlyContinue }"
        Write-Host 'Unbound. Windows no longer has a key for this controller.'
        return
    }

    # 3. key
    $key = New-Object byte[] 16
    (New-Object Security.Cryptography.RNGCryptoServiceProvider).GetBytes($key)
    $wireKey = if ($ReverseKey) { [byte[]]($key[15..0]) } else { $key }
    $keyLiteral = ($key | ForEach-Object { '0x{0:X2}' -f $_ }) -join ','

    # 4. store in Windows key store (do not use New-Item -Force on an existing key: it wipes other pairings)
    Write-Host 'Storing link key in the Windows Bluetooth key store (as SYSTEM)...'
    Invoke-AsSystem @"
if (-not (Test-Path '$regPath')) { New-Item -Path '$regPath' | Out-Null }
New-ItemProperty -Path '$regPath' -Name '$devName' -PropertyType Binary -Value ([byte[]]($keyLiteral)) -Force | Out-Null
"@

    # 5. make the stack load it
    Write-Host 'Restarting Bluetooth so Windows loads the key...'
    $h.Close()
    Restart-BluetoothRadio

    # 6. give the S5 this PC's address + key
    $path = [S5Native]::HidPaths() | Where-Object { $_ -match 'vid_054c&pid_0ce6' -and $_ -notmatch '00001124' } | Select-Object -First 1
    if (-not $path) { throw 'Controller disappeared from USB during setup. Replug it and run again.' }
    $h = [S5Native]::Open($path)
    $radioLE = [BitConverter]::GetBytes($radio)[0..5]
    $r0A = New-Object byte[] 27; $r0A[0] = 0x0A
    [Array]::Copy([byte[]]$radioLE, 0, $r0A, 1, 6); [Array]::Copy([byte[]]$wireKey, 0, $r0A, 7, 16)
    [S5Native]::SetFeature($h, $r0A)
    Start-Sleep -Milliseconds 200
    $r08 = New-Object byte[] 48; $r08[0] = 0x08; $r08[1] = 0x01
    [S5Native]::SetFeature($h, $r08)
    $after = [S5Native]::GetFeature($h, 0x09)
    [UInt64]$hostAddr = ConvertFrom-LEAddr ([byte[]]$after[10..15])
    Write-Host "S5 now reports host: $(Format-Addr $hostAddr)  (GP2040 may cache this until replug)"
    $h.Close()

    # 7. ask Windows to connect and install the HID driver
    if (-not $NoConnect) {
        Start-Sleep -Seconds 2
        [UInt64]$r2 = 0; $hRadio = [S5Native]::Radio([ref]$r2)
        $rc = [S5Native]::EnableHid($hRadio, $dev)
        [S5Native]::CloseHandle($hRadio) | Out-Null
        Write-Host "Enable HID service result: $rc  (0 = success)"
    }

    Write-Host ''
    Write-Host "Paired. The controller ($(Format-Addr $dev)) is now bound to this PC ($env:COMPUTERNAME, $(Format-Addr $radio))." -ForegroundColor Green
    Write-Host 'You can unplug it. Whenever it is powered on, it will connect wirelessly to this PC'
    Write-Host 'until you remove it from this PC (Settings > Bluetooth & devices, or run this file with -Unbind)'
    Write-Host 'or pair it to a different host (PS5, Mac, or running this tool on another PC).'
    Write-Host ''
    Write-Host 'If it does not connect wirelessly, reboot this PC and power-cycle the controller.'
} catch {
    Write-Host "ERROR: $_" -ForegroundColor Red
} finally {
    Read-Host 'Press Enter to close'
}
