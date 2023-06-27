/*  Copyright (C) 2019-2020 Ken Chaffin
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Common-Noise.hpp" 

bool doDebug = false;  // set this to true to enable verbose DEBUG() logging

bool globalsInitialized=false;  // globals as initialized only during Module(), otherwise the global values are whatever they were set to here, if anything.

bool Audit_enable=false;  

using namespace rack;

#define CV_MAX10 (10.0f)
#define CV_MAXn5 (5.0f)
#define AUDIO_MAX (6.0f)
#define VOCT_MAX (8.0f)
#define AMP_MAX (2.0f)       
 
#define MAX_NOTES 12
#define MAX_STEPS 16
#define MAX_CIRCLE_STATIONS 12
#define MAX_HARMONIC_DEGREES 7
#define MAX_AVAILABLE_HARMONY_PRESETS 59  // change this as new harmony presets are created

#define MAX_PARAMS 200
#define MAX_INPORTS 100
#define MAX_OUTPORTS 100


struct TinyPJ301MPort : SvgPort {
	TinyPJ301MPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyPJ301M.svg")));
	}
};

// make it a power of 8
#define MAXSHORTSTRLEN 16

#define MAX_ROOT_KEYS 12

enum noteTypes
{
	NOTE_TYPE_CHORD,
	NOTE_TYPE_MELODY,
	NOTE_TYPE_ARP,
	NOTE_TYPE_BASS,
	NOTE_TYPE_EXTERNAL
};

static const char root_key_names[MAX_ROOT_KEYS][MAXSHORTSTRLEN] = { "C","Db","D","Eb","E","F","F#","G","Ab","A","Bb","B" };


#define MAX_MODES 7
static const int num_modes=MAX_MODES;
static const char mode_names[MAX_MODES][16] = {
	"Lydian",
	"Ionian/Major",
	"Mixolydian",
	"Dorian",
	"Aeolian/NMinor",
	"Phrygian",
	"Locrian"
};

static const int mode_step_intervals[7][13] = {  // num mode scale notes, semitones to next note  7 modes
	{ 7, 2,2,2,1,2,2,1,0,0,0,0,0},                // Lydian  	        
	{ 7, 2,2,1,2,2,2,1,0,0,0,0,0},                // Major/Ionian      
	{ 7, 2,2,1,2,2,1,2,0,0,0,0,0},                // Mixolydian	   
	{ 7, 2,1,2,2,2,1,2,0,0,0,0,0},                // Dorian           
	{ 7, 2,1,2,2,1,2,2,0,0,0,0,0},                // NMinor/Aeolian   
	{ 7, 1,2,2,2,1,2,2,0,0,0,0,0},                // Phrygian         
	{ 7, 1,2,2,1,2,2,2,0,0,0,0,0}                 // Locrian            
}; 

static const int root_key_signatures_chromaticForder[12][7]=  // chromatic order 0=natural, 1=sharp, -1=flat
{  // F  C  G  D  A  E  B  in order of root_key signature sharps and flats starting with F
  	{ 0, 0, 0, 0, 0, 0, 0},  // C  - 0
	{ 0, 0,-1,-1,-1,-1,-1},  // Db - 1
	{ 1, 1, 0, 0, 0, 0, 0},  // D  - 2
	{ 0, 0, 0, 0,-1,-1,-1},  // Eb - 3
	{ 1, 1, 1, 1, 0, 0, 0},  // E  - 4
	{ 0, 0, 0, 0, 0, 0,-1},  // F  - 5
	{ 1, 1, 1, 1, 1, 1, 0},  // F# - 6
	{ 1, 0, 0, 0, 0, 0, 0},  // G  - 7
	{ 0, 0, 0,-1,-1,-1,-1},  // Ab - 8
	{ 1, 1, 1, 0, 0, 0, 0},  // A  - 9
	{ 0, 0, 0, 0, 0,-1,-1},  // Bb - 10
	{ 1, 1, 1, 1, 1, 0, 0},  // B  - 11
	
};

static const char* noteNames[MAX_NOTES] = {"C","C#/Db","D","D#/Eb","E","F","F#/Gb","G","G#/Ab","A","A#/Bb","B"};
static const char* CircleNoteNames[MAX_NOTES] = {"C","G","D","A","E","B","F#","Db","Ab","Eb","Bb","F"};

static const char circle_of_fifths_degrees[][MAXSHORTSTRLEN]= {
	"I", "V", "II", "vi", "iii", "vii", "IV"
};

static const char circle_of_fifths_arabic_degrees[][MAXSHORTSTRLEN]= {
	"", "I", "II", "III", "IV", "V", "IV", "VII"
};

static const char circle_of_fifths_degrees_UC[][MAXSHORTSTRLEN]= {
	"I", "V", "II", "VI", "III", "VII", "IV"
};

static const char circle_of_fifths_degrees_LC[][MAXSHORTSTRLEN]= {
	"i", "v", "ii", "vi", "iii", "vii", "iv"
};

static const int root_key_sharps_vertical_display_offset[6]={1, 4, 0, 3, 6, 2};  // left to right
static const int root_key_flats_vertical_display_offset[6]={5, 2, 6, 3, 7, 4};   // right to left

