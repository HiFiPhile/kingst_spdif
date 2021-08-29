#ifndef SPDIF_ANALYZER_SETTINGS
#define SPDIF_ANALYZER_SETTINGS
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

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

enum spdifRes
{
    spdifRes_16b = 0,
    spdifRes_24b,
};

class spdifAnalyzerSettings : public AnalyzerSettings
{
public:
    spdifAnalyzerSettings();
    virtual ~spdifAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    void UpdateInterfacesFromSettings();
    virtual void LoadSettings( const char* settings );
    virtual const char* SaveSettings();

    Channel mInputChannel;
    spdifRes mRes;

protected:
    std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
    std::auto_ptr< AnalyzerSettingInterfaceNumberList > mResInterface;
};

#endif //SPDIF_ANALYZER_SETTINGS
