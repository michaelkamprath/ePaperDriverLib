//     ePaper Driver Lib for Arduino Project
//     Copyright (C) 2019 Michael Kamprath
//
//     This file is part of ePaper Driver Lib for Arduino Project.
// 
//     ePaper Driver Lib for Arduino Project is free software: you can 
//	   redistribute it and/or modify it under the terms of the GNU General Public License
//     as published by the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     ePaper Driver Lib for Arduino Project is distributed in the hope that 
// 	   it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.
//
//     This project and its creators are not associated with Crystalfontz, Good display
//	   or any other manufacturer, nor is this  project officially endorsed or reviewed for 
//	   correctness by any ePaper manufacturer. 
//
#include <Arduino.h>
#include "ePaperDeviceConfigurations.h"

const uint8_t* ePaperDeviceConfigurations::deviceConfiguration(ePaperDeviceModel model)
{
	switch (model) {
		case CFAP104212C0_0213:
		case CFAP104212E0_0213:
			return deviceConfiguration_CFAP104212E0_0213;
			break;
		case CFAP176264A0_0270:
			return deviceConfiguration_CFAP176264A0_0270;
			break;
		case CFAP400300C0_420:
		case CFAP400300A0_420:
			return deviceConfiguration_CFAP4003002A0_0420;
			break;
		default:
			return 0;
			break;
	}
}

uint8_t ePaperDeviceConfigurations::deviceConfigurationSize(ePaperDeviceModel model)
{
	switch (model) {
		case CFAP104212C0_0213:
		case CFAP104212E0_0213:
			return pgm_read_byte(&deviceConfigurationSize_CFAP104212E0_0213);
			break;
		case CFAP176264A0_0270:
			return pgm_read_byte(&deviceConfigurationSize_CFAP176264A0_0270);
			break;
		case CFAP400300A0_420:
		case CFAP400300C0_420:
			return pgm_read_byte(&deviceConfigurationSize_CFAP4003002A0_0420);
			break;
		default:
			return 0;
			break;
	}
}

int ePaperDeviceConfigurations::deviceSizeVertical(ePaperDeviceModel model)
{
	switch (model) {
		case CFAP104212C0_0213:
		case CFAP104212E0_0213:
			return 212;
			break;
		case CFAP176264A0_0270:
			return 264;
			break;
		case CFAP400300A0_420:
		case CFAP400300C0_420:
			return 300;
			break;
		default:
			return 0;
			break;
	}
}

int ePaperDeviceConfigurations::deviceSizeHorizontal(ePaperDeviceModel model)
{
	switch (model) {
		case CFAP104212C0_0213:
		case CFAP104212E0_0213:
			return 104;
			break;
		case CFAP176264A0_0270:
			return 176;
			break;
		case CFAP400300A0_420:
		case CFAP400300C0_420:
			return 400;
			break;
		default:
			return 0;
			break;
	}
}

bool ePaperDeviceConfigurations::deviceHasThirdColor(ePaperDeviceModel model)
{
	switch (model) {
		case CFAP104212C0_0213:
		case CFAP104212E0_0213:
		case CFAP176264A0_0270:
		case CFAP400300A0_420:
		case CFAP400300C0_420:
			return true;
			break;
		default:
			return false;
			break;
	}
}
