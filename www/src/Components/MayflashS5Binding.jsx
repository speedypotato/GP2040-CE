import { useState } from 'react';
import { Col, Form, Row } from 'react-bootstrap';
import { Trans, useTranslation } from 'react-i18next';

// Windows has no built-in cable pairing for the S5, so the web config serves an
// offline helper. It lives in www/public/downloads and is packed into fsdata.
const WINDOWS_HELPER = '/downloads/S5-Bind-Windows.cmd';

const PLATFORMS = ['ps5', 'macos', 'ios', 'windows', 'other'];

const listComponents = {
	ol: <ol className="mb-2" />,
	ul: <ul className="mb-2" />,
	li: <li />,
	strong: <strong />,
	code: <code />,
};

export default function MayflashS5Binding() {
	const { t } = useTranslation('');
	const [platform, setPlatform] = useState('ps5');

	return (
		<div className="mb-3">
			<h6 className="mb-2">{t('SettingsPage:mayflash-s5-binding.header')}</h6>
			<p className="mb-2">{t('SettingsPage:mayflash-s5-binding.intro')}</p>
			<Row className="mb-3">
				<Col sm={4}>
					<Form.Label htmlFor="mayflash-s5-binding-platform">
						{t('SettingsPage:mayflash-s5-binding.platform-label')}
					</Form.Label>
					<Form.Select
						id="mayflash-s5-binding-platform"
						className="form-select-sm"
						value={platform}
						onChange={(e) => setPlatform(e.target.value)}
					>
						{PLATFORMS.map((p) => (
							<option key={`mayflash-s5-binding-${p}`} value={p}>
								{t(`SettingsPage:mayflash-s5-binding.platforms.${p}`)}
							</option>
						))}
					</Form.Select>
				</Col>
			</Row>
			{platform !== 'other' && (
				<>
					<strong>{t('SettingsPage:mayflash-s5-binding.bind-label')}</strong>
					<Trans
						ns="SettingsPage"
						i18nKey={`mayflash-s5-binding.${platform}.bind`}
						components={listComponents}
					/>
					{platform === 'windows' && (
						<p className="mb-2">
							<a
								className="btn btn-sm btn-primary"
								href={WINDOWS_HELPER}
								download="S5-Bind-Windows.cmd"
							>
								{t('SettingsPage:mayflash-s5-binding.windows.download')}
							</a>
						</p>
					)}
					<strong>{t('SettingsPage:mayflash-s5-binding.unbind-label')}</strong>
					<Trans
						ns="SettingsPage"
						i18nKey={`mayflash-s5-binding.${platform}.unbind`}
						components={listComponents}
					/>
				</>
			)}
			{platform === 'other' && (
				<Trans
					ns="SettingsPage"
					i18nKey="mayflash-s5-binding.other.text"
					components={listComponents}
				/>
			)}
			<p className="mb-0 text-info">
				{t('SettingsPage:mayflash-s5-binding.one-host-note')}
			</p>
		</div>
	);
}
