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
#include "spdifAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "spdifAnalyzer.h"
#include "spdifAnalyzerSettings.h"
#include <iostream>
#include <fstream>

spdifAnalyzerResults::spdifAnalyzerResults( spdifAnalyzer* analyzer, spdifAnalyzerSettings* settings )
:	AnalyzerResults(),
    mSettings( settings ),
    mAnalyzer( analyzer )
{
}

spdifAnalyzerResults::~spdifAnalyzerResults()
{
}

void spdifAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
    Frame frame = GetFrame(frame_index);
    const char* frtype;

    ClearResultStrings();

    char number_str[128];

    switch (frame.mType)
    {
    case sft_B:
        frtype = "B";
        break;
    case (sft_B | 0x80):
        frtype = "B";
        break;

    case sft_M:
        frtype = "M";
        break;

    case sft_W:
        frtype = "W";
        break;

    case sft_invalid:
    default:
        frtype = "Err";
        break;
    }

    const char *v, *u, *c;
    if (!(frame.mData2 & 1 << 28))
    {
        v = " + V";
    }
    else
    {
        v = " + NV";
    }
    //if (frame.mData2 & 1 << 29)
    //{
    //    u = " + U";
    //}
    //else
    //{
    //    u = "";
    //}
    if (frame.mData2 & 1 << 30)
    {
        c = " + 1";
    }
    else
    {
        c = " + 0";
    }

    if (mSettings->mRes == spdifRes_16b)
        AnalyzerHelpers::GetNumberString(frame.mData1 & 0x0000ffff, display_base, 16, number_str, 128);
    else
        AnalyzerHelpers::GetNumberString(frame.mData1 & 0x00ffffff, display_base, 24, number_str, 128);

    char result_str[128];

    snprintf(result_str, sizeof(result_str), "%s + %s%s%s", frtype, number_str, v, c);

    AddResultString(number_str);
    AddResultString(result_str);
}

void spdifAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
    if ( 0 == export_type_user_id ) /* text/csv */
    {
        std::ofstream file_stream( file, std::ios::out );

        U64 trigger_sample = mAnalyzer->GetTriggerSample();
        U32 sample_rate = mAnalyzer->GetSampleRate();

        file_stream << "Time [s],Value" << std::endl;

        U64 num_frames = GetNumFrames();
        for( U32 i=0; i < num_frames; i++ )
        {
            Frame frame = GetFrame( i );
            
            char time_str[128];
            AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

            char number_str[128];
            if (mSettings->mRes == spdifRes_16b)
                AnalyzerHelpers::GetNumberString(frame.mData1 & 0x0000ffff, display_base, 16, number_str, 128);
            else
                AnalyzerHelpers::GetNumberString(frame.mData1 & 0x00ffffff, display_base, 24, number_str, 128);

            file_stream << time_str << "," << number_str << std::endl;

            if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
            {
                break;
            }
        }
        file_stream.close();
    }
    else if ( 1 == export_type_user_id ) /* wav */
    {
        std::ofstream file_stream( file, std::ios::binary );

        struct WAVHeader     wh;
        U64                  num_frames = GetNumFrames();
        U64                  num_samples = 0;

        wh_Init( &wh, (uint32_t) num_frames>>1 );

        file_stream.write((const char *)&wh,sizeof(wh));

        for( U32 i=0; i < num_frames; i++ )
        {
            Frame frame = GetFrame( i );
            
            if ( ! (0x80 & frame.mType) ) /* general errors are not PCM */
            {
                uint16_t pcm = (uint16_t) frame.mData1;
                file_stream.write((const char *)&pcm,sizeof(pcm));
                num_samples++;
            }

            if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
            {
                break;
            }
        }

        wh_Init( &wh, (uint32_t) num_samples>>1 );
        file_stream.seekp(0);
        file_stream.write((const char *)&wh,sizeof(wh));
        file_stream.close();
    }
    else if ( 2 == export_type_user_id ) /* raw/bin */
    {
        std::ofstream file_stream( file, std::ios::binary );

        U64                  num_frames = GetNumFrames();

        for( U32 i=0; i < num_frames; i++ )
        {
            Frame frame = GetFrame( i );
            
            if ( ! (0x80 & frame.mType) ) /* general errors are not data */
            {
                uint32_t raw = (uint32_t) frame.mData2;

                file_stream.write((const char *)&raw,sizeof(raw));
            }

            if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
            {
                break;
            }
        }
        file_stream.close();
    }

}

void spdifAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
    Frame frame = GetFrame( frame_index );
    const char *frtype;

    ClearTabularText();

    char number_str[128];

    switch(frame.mType)
    {
        case sft_B:
            frtype = "B";
        break;
        case (sft_B | 0x80):
            frtype = "B";
        break;

        case sft_M:
            frtype = "M";
        break;

        case sft_W:
            frtype = "W";
        break;

        case sft_invalid:
        default:
            frtype = "Err";
        break;
    }

    const char* v, * u, * c;
    if (!(frame.mData2 & 1 << 28))
    {
        v = " + V";
    }
    else
    {
        v = " + NV";
    }
    //if (frame.mData2 & 1 << 29)
    //{
    //    u = " + U";
    //}
    //else
    //{
    //    u = "";
    //}
    if (frame.mData2 & 1 << 30)
    {
        c = " + 1";
    }
    else
    {
        c = " + 0";
    }

    if (mSettings->mRes == spdifRes_16b)
        AnalyzerHelpers::GetNumberString(frame.mData1 & 0x0000ffff, display_base, 16, number_str, 128);
    else
        AnalyzerHelpers::GetNumberString(frame.mData1 & 0x00ffffff, display_base, 24, number_str, 128);

    char result_str[128];

    snprintf(result_str, sizeof(result_str), "%s + %s%s%s", frtype, number_str, v, c);

    AddTabularText(result_str);
}

void spdifAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
    ClearResultStrings();
    AddResultString( "not supported" );
}

void spdifAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
    ClearResultStrings();
    AddResultString( "not supported" );
}