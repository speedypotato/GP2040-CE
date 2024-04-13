import React from 'react';
import { useTranslation } from 'react-i18next';
import * as yup from 'yup';

import Section from '../Components/Section';
import FormSelect from '../Components/FormSelect';
import FormControl from '../Components/FormControl';
import { FormCheck, Row, Col } from 'react-bootstrap';

export const invertButtonScheme = {
	InvertButtonAddonEnabled: yup
		.number()
		.required()
		.label('Invert Button Add-On Enabled'),
    invertB1: yup
        .boolean()
        .required()
		.label('Invert B1 Enabled'),
    invertB2: yup
        .boolean()
        .required()
        .label('Invert B2 Enabled'),
    invertB3: yup
        .boolean()
        .required()
        .label('Invert B3 Enabled'),
    invertB4: yup
        .boolean()
        .required()
        .label('Invert B4 Enabled'),
    invertL1: yup
        .boolean()
        .required()
        .label('Invert L1 Enabled'),
    invertR1: yup
        .boolean()
        .required()
        .label('Invert R1 Enabled'),
    invertL2: yup
        .boolean()
        .required()
        .label('Invert L2 Enabled'),
    invertR2: yup
        .boolean()
        .required()
        .label('Invert R2 Enabled'),
    invertS1: yup
        .boolean()
        .required()
        .label('Invert S1 Enabled'),
    invertS2: yup
        .boolean()
        .required()
        .label('Invert S2 Enabled'),
    invertL3: yup
        .boolean()
        .required()
        .label('Invert L3 Enabled'),
    invertR3: yup
        .boolean()
        .required()
        .label('Invert R3 Enabled'),
    invertA1: yup
        .boolean()
        .required()
        .label('Invert A1 Enabled'),
    invertA2: yup
        .boolean()
        .required()
        .label('Invert A2 Enabled'),
    invertUp: yup
        .boolean()
        .required()
        .label('Invert Up Enabled'),
    invertDown: yup
        .boolean()
        .required()
        .label('Invert Down Enabled'),
    invertLeft: yup
        .boolean()
        .required()
        .label('Invert Left Enabled'),
    invertRight: yup
        .boolean()
        .required()
        .label('Invert Right Enabled'),
};

export const invertButtonState = {
    invertB1: false,
    invertB2: false,
    invertB3: false,
    invertB4: false,
    invertL1: false,
    invertR1: false,
    invertL2: false,
    invertR2: false,
    invertS1: false,
    invertS2: false,
    invertL3: false,
    invertR3: false,
    invertA1: false,
    invertA2: false,
    invertUp: false,
    invertDown: false,
    invertLeft: false,
    invertRight: false,
    InvertButtonAddonEnabled: 0,
};

const InvertButton = ({ values, errors, handleChange, handleCheckbox }) => {
	const { t } = useTranslation();
	return (
		<Section title={t('InvertButton:header-text')}>
			<div
				id="InvertButtonAddonEnabledOptions"
				hidden={!values.InvertButtonAddonEnabled}
			>
                <Row className="mb-3">
                    <div className="col">
                        <label>{t('InvertButton:invert-b1-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertB1"
                            isInvalid={false}
                            checked={Boolean(values.invertB1)}
                            value={Boolean(values.invertB1)}
                            onChange={(e) => {
                                handleCheckbox('invertB1', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-b2-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertB2"
                            isInvalid={false}
                            checked={Boolean(values.invertB2)}
                            value={Boolean(values.invertB2)}
                            onChange={(e) => {
                                handleCheckbox('invertB2', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-b3-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertB3"
                            isInvalid={false}
                            checked={Boolean(values.invertB3)}
                            value={Boolean(values.invertB3)}
                            onChange={(e) => {
                                handleCheckbox('invertB3', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-b4-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertB4"
                            isInvalid={false}
                            checked={Boolean(values.invertB4)}
                            value={Boolean(values.invertB4)}
                            onChange={(e) => {
                                handleCheckbox('invertB4', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                </Row>
                <Row className="mb-3">
                    <div className="col">
                        <label>{t('InvertButton:invert-l1-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertL1"
                            isInvalid={false}
                            checked={Boolean(values.invertL1)}
                            value={Boolean(values.invertL1)}
                            onChange={(e) => {
                                handleCheckbox('invertL1', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-r1-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertR1"
                            isInvalid={false}
                            checked={Boolean(values.invertR1)}
                            value={Boolean(values.invertR1)}
                            onChange={(e) => {
                                handleCheckbox('invertR1', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-l2-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertL2"
                            isInvalid={false}
                            checked={Boolean(values.invertL2)}
                            value={Boolean(values.invertL2)}
                            onChange={(e) => {
                                handleCheckbox('invertL2', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-r2-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertR2"
                            isInvalid={false}
                            checked={Boolean(values.invertR2)}
                            value={Boolean(values.invertR2)}
                            onChange={(e) => {
                                handleCheckbox('invertR2', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                </Row>
                <Row className="mb-3">
                    <div className="col">
                        <label>{t('InvertButton:invert-s1-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertS1"
                            isInvalid={false}
                            checked={Boolean(values.invertS1)}
                            value={Boolean(values.invertS1)}
                            onChange={(e) => {
                                handleCheckbox('invertS1', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-s2-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertS2"
                            isInvalid={false}
                            checked={Boolean(values.invertS2)}
                            value={Boolean(values.invertS2)}
                            onChange={(e) => {
                                handleCheckbox('invertS2', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-l3-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertL3"
                            isInvalid={false}
                            checked={Boolean(values.invertL3)}
                            value={Boolean(values.invertL3)}
                            onChange={(e) => {
                                handleCheckbox('invertL3', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-r3-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertR3"
                            isInvalid={false}
                            checked={Boolean(values.invertR3)}
                            value={Boolean(values.invertR3)}
                            onChange={(e) => {
                                handleCheckbox('invertR3', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-a1-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertA1"
                            isInvalid={false}
                            checked={Boolean(values.invertA1)}
                            value={Boolean(values.invertA1)}
                            onChange={(e) => {
                                handleCheckbox('invertA1', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-a2-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertA2"
                            isInvalid={false}
                            checked={Boolean(values.invertA2)}
                            value={Boolean(values.invertA2)}
                            onChange={(e) => {
                                handleCheckbox('invertA2', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                </Row>
                <Row className="mb-3">
                    <div className="col">
                        <label>{t('InvertButton:invert-up-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertUp"
                            isInvalid={false}
                            checked={Boolean(values.invertUp)}
                            value={Boolean(values.invertUp)}
                            onChange={(e) => {
                                handleCheckbox('invertUp', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-down-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertDown"
                            isInvalid={false}
                            checked={Boolean(values.invertDown)}
                            value={Boolean(values.invertDown)}
                            onChange={(e) => {
                                handleCheckbox('invertDown', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-left-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertLeft"
                            isInvalid={false}
                            checked={Boolean(values.invertLeft)}
                            value={Boolean(values.invertLeft)}
                            onChange={(e) => {
                                handleCheckbox('invertLeft', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                    <div className="col">
                        <label>{t('InvertButton:invert-right-label')}</label>
                        <FormCheck
                            label={t('Common:switch-enabled')}
                            type="switch"
                            id="invertRight"
                            isInvalid={false}
                            checked={Boolean(values.invertRight)}
                            value={Boolean(values.invertRight)}
                            onChange={(e) => {
                                handleCheckbox('invertRight', values);
                                handleChange(e);
                            }}
                        />
                    </div>
                </Row>
			</div>
			<FormCheck
				label={t('Common:switch-enabled')}
				type="switch"
				id="InvertButtonAddonButton"
				reverse
				isInvalid={false}
				checked={Boolean(values.InvertButtonAddonEnabled)}
				onChange={(e) => {
					handleCheckbox('InvertButtonAddonEnabled', values);
					handleChange(e);
				}}
			/>
		</Section>
	);
};

export default InvertButton;
