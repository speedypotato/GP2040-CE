import React from 'react';
import { FormCheck, Row } from 'react-bootstrap';
import { useTranslation } from 'react-i18next';
import * as yup from 'yup';

import FormControl from '../Components/FormControl';
import Section from '../Components/Section';

export const sdvxScheme = {
	SdvxInputEnabled: yup.number().required().label('SDVX Input Enabled'),
	lKnobA: yup
		.number()
		.label('Left Knob Ch A')
		.validatePinWhenValue('SdvxInputEnabled'),
	lKnobB: yup
		.number()
		.label('Left Knob Ch B')
		.validatePinWhenValue('SdvxInputEnabled'),
	rKnobA: yup
		.number()
		.label('Right Knob Ch A')
		.validatePinWhenValue('SdvxInputEnabled'),
	rKnobB: yup
		.number()
		.label('Right Knob Ch B')
		.validatePinWhenValue('SdvxInputEnabled'),
};

export const sdvxState = {
	SdvxInputEnabled: 0,
	lKnobA: 27,
	lKnobB: 28,
	rKnobA: 6,
	rKnobB: 7,
};

const Sdvx = ({ values, errors, handleChange, handleCheckbox }) => {
	const { t } = useTranslation();
	return (
		<Section title={'SDVX'}>
			<div id="SdvxInputOptions" hidden={!values.SdvxInputEnabled}>
				<Row className="mb-3">
					<FormControl
						type="number"
						label={'Left Knob Ch A'}
						name="lKnobA"
						className="form-select-sm"
						groupClassName="col-sm-3 mb-3"
						value={values.lKnobA}
						error={errors.lKnobA}
						isInvalid={errors.lKnobA}
						onChange={handleChange}
						min={-1}
						max={21}
					/>
					<FormControl
						type="number"
						label={'Left Knob Ch B'}
						name="lKnobB"
						className="form-select-sm"
						groupClassName="col-sm-3 mb-3"
						value={values.lKnobB}
						error={errors.lKnobB}
						isInvalid={errors.lKnobB}
						onChange={handleChange}
						min={-1}
						max={21}
					/>
					<FormControl
						type="number"
						label={'Right Knob Ch A'}
						name="rKnobA"
						className="form-select-sm"
						groupClassName="col-sm-3 mb-3"
						value={values.rKnobA}
						error={errors.rKnobA}
						isInvalid={errors.rKnobA}
						onChange={handleChange}
						min={-1}
						max={21}
					/>
					<FormControl
						type="number"
						label={'Right Knob Ch B'}
						name="rKnobB"
						className="form-select-sm"
						groupClassName="col-sm-3 mb-3"
						value={values.rKnobB}
						error={errors.rKnobB}
						isInvalid={errors.rKnobB}
						onChange={handleChange}
						min={-1}
						max={21}
					/>
				</Row>
			</div>
			<FormCheck
				label={t('Common:switch-enabled')}
				type="switch"
				id="SdvxInputButton"
				reverse
				isInvalid={false}
				checked={Boolean(values.SdvxInputEnabled)}
				onChange={(e) => {
					handleCheckbox('SdvxInputEnabled', values);
					handleChange(e);
				}}
			/>
		</Section>
	);
};

export default Sdvx;
