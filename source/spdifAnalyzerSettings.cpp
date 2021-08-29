#include "spdifAnalyzerSettings.h"
#include <AnalyzerHelpers.h>
/* 

  Copyright(c) HiFiPhile. All rights reserved.

  This file contains a modified version of Pat Brouillette's original work, below is the license:

  GPL LICENSE SUMMARY

  Copyright(c) Pat Brouillette. All rights reserved.

  This program is free software; you can redistribute it and/or modify 
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
  General Public License for more details.

  You should have received a copy of the GNU General Public License 
  along with this program; if not, write to the Free Software 
  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
  The full GNU General Public License is included in this distribution 
  in the file called LICENSE.GPL.

  Contact Information:
    Pat Brouillette  pfrench@acm.org

*/


spdifAnalyzerSettings::spdifAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ), mRes( spdifRes_16b )
{
	mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mInputChannelInterface->SetTitleAndTooltip( "SPDIF", "Standard Pat's SPDIF" );
	mInputChannelInterface->SetChannel( mInputChannel );

	AddInterface( mInputChannelInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

    AddExportOption( 1, "Export as wav file" );
    AddExportExtension( 1, "wav", "wav" );

    AddExportOption( 2, "Export as raw file" );
    AddExportExtension( 2, "raw", "raw" );
    AddExportExtension( 2, "bin", "bin" );

	ClearChannels();
	AddChannel( mInputChannel, "SPDIF", false );

	mResInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mResInterface->SetTitleAndTooltip("Resolution", "");
	mResInterface->AddNumber(spdifRes_16b, "16 Bit", "");
	mResInterface->AddNumber(spdifRes_24b, "24 Bit", "");
	mResInterface->SetNumber(mRes);

	AddInterface(mResInterface.get());
}

spdifAnalyzerSettings::~spdifAnalyzerSettings()
{
}

bool spdifAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface->GetChannel();

	ClearChannels();
	AddChannel( mInputChannel, "SPDIF analyzer", true );

	mRes = (spdifRes)mResInterface->GetNumber();

	return true;
}

void spdifAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface->SetChannel( mInputChannel );
	mResInterface->SetNumber(mRes);
}

void spdifAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;

	text_archive >> *(U32*)&mRes;

	ClearChannels();
	AddChannel( mInputChannel, "SPDIF analyzer", true );

	mResInterface->SetNumber(mRes);

	UpdateInterfacesFromSettings();
}

const char* spdifAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;

	text_archive << mRes;

	return SetReturnString( text_archive.GetString() );
}
