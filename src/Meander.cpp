/*  Copyright (C) 2019-2020 Ken Chaffin
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <rack.hpp>   
   

#include "plugin.hpp"  
#include <sstream> 
#include <iomanip>
#include <time.h>

#include <iostream>
#include <fstream> 
#include <string>
#include <mutex>

#include "Meander.hpp"

// the following is for performance profiling under Windows by the developer and should not be defined in the release distribution
// #define _USE_WINDOWS_PERFTIME

#if defined(_USE_WINDOWS_PERFTIME)
#include <windows.h>
#endif


struct Meander : Module 
{
	bool instanceRunning = false;
	
	enum ParamIds 
	{
		BUTTON_RUN_PARAM,
		BUTTON_RESET_PARAM,

		CONTROL_TEMPOBPM_PARAM,
		CONTROL_TIMESIGNATURETOP_PARAM,
		CONTROL_TIMESIGNATUREBOTTOM_PARAM,

		CONTROL_ROOT_KEY_PARAM,
		CONTROL_SCALE_PARAM,

		BUTTON_CIRCLESTEP_C_PARAM,
		BUTTON_CIRCLESTEP_G_PARAM,
		BUTTON_CIRCLESTEP_D_PARAM,
		BUTTON_CIRCLESTEP_A_PARAM,
		BUTTON_CIRCLESTEP_E_PARAM,
		BUTTON_CIRCLESTEP_B_PARAM,
		BUTTON_CIRCLESTEP_GBFS_PARAM,
		BUTTON_CIRCLESTEP_DB_PARAM,
		BUTTON_CIRCLESTEP_AB_PARAM,
		BUTTON_CIRCLESTEP_EB_PARAM,
		BUTTON_CIRCLESTEP_BB_PARAM,
		BUTTON_CIRCLESTEP_F_PARAM,

	
		BUTTON_HARMONY_SETSTEP_1_PARAM,
		BUTTON_HARMONY_SETSTEP_2_PARAM,
		BUTTON_HARMONY_SETSTEP_3_PARAM,
		BUTTON_HARMONY_SETSTEP_4_PARAM,
		BUTTON_HARMONY_SETSTEP_5_PARAM,
		BUTTON_HARMONY_SETSTEP_6_PARAM,
		BUTTON_HARMONY_SETSTEP_7_PARAM,
		BUTTON_HARMONY_SETSTEP_8_PARAM,
		BUTTON_HARMONY_SETSTEP_9_PARAM,
		BUTTON_HARMONY_SETSTEP_10_PARAM,
		BUTTON_HARMONY_SETSTEP_11_PARAM,
		BUTTON_HARMONY_SETSTEP_12_PARAM,
		BUTTON_HARMONY_SETSTEP_13_PARAM,
		BUTTON_HARMONY_SETSTEP_14_PARAM,
		BUTTON_HARMONY_SETSTEP_15_PARAM,
		BUTTON_HARMONY_SETSTEP_16_PARAM,

		BUTTON_ENABLE_MELODY_PARAM,
		CONTROL_MELODY_VOLUME_PARAM,
		BUTTON_MELODY_DESTUTTER_PARAM,
		CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM,
		CONTROL_MELODY_TARGETOCTAVE_PARAM,
		CONTROL_MELODY_ALPHA_PARAM,
		CONTROL_MELODY_RANGE_PARAM,


		BUTTON_ENABLE_ARP_PARAM,
		CONTROL_ARP_COUNT_PARAM,
		CONTROL_ARP_INCREMENT_PARAM,
		CONTROL_ARP_DECAY_PARAM,
		CONTROL_ARP_PATTERN_PARAM,
		BUTTON_ENABLE_ARP_CHORDAL_PARAM,
		BUTTON_ENABLE_ARP_SCALER_PARAM,


		BUTTON_ENABLE_HARMONY_PARAM,
		BUTTON_HARMONY_DESTUTTER_PARAM,
		CONTROL_HARMONY_VOLUME_PARAM,
		CONTROL_HARMONY_STEPS_PARAM,
		CONTROL_HARMONY_TARGETOCTAVE_PARAM,
		CONTROL_HARMONY_ALPHA_PARAM,
		CONTROL_HARMONY_RANGE_PARAM,
		CONTROL_HARMONY_DIVISOR_PARAM,
		CONTROL_HARMONYPRESETS_PARAM,

		BUTTON_ENABLE_BASS_PARAM,
		CONTROL_BASS_VOLUME_PARAM,
		CONTROL_BASS_TARGETOCTAVE_PARAM,
		BUTTON_BASS_ACCENT_PARAM,
		BUTTON_BASS_SYNCOPATE_PARAM,
		BUTTON_BASS_SHUFFLE_PARAM,
		CONTROL_BASS_DIVISOR_PARAM,

				
		CONTROL_HARMONY_FBM_OCTAVES_PARAM,
		CONTROL_MELODY_FBM_OCTAVES_PARAM,
		CONTROL_ARP_FBM_OCTAVES_PARAM,

		CONTROL_HARMONY_FBM_PERIOD_PARAM,
		CONTROL_MELODY_FBM_PERIOD_PARAM,
		CONTROL_ARP_FBM_PERIOD_PARAM,

		BUTTON_ENABLE_MELODY_CHORDAL_PARAM,
		BUTTON_ENABLE_MELODY_SCALER_PARAM,

		BUTTON_ENABLE_HARMONY_ALL7THS_PARAM,
		BUTTON_ENABLE_HARMONY_V7THS_PARAM,

		BUTTON_ENABLE_HARMONY_STACCATO_PARAM,
		BUTTON_ENABLE_MELODY_STACCATO_PARAM,
		BUTTON_ENABLE_BASS_STACCATO_PARAM,

		BUTTON_BASS_OCTAVES_PARAM,

		BUTTON_PROG_STEP_PARAM,
				
		
		NUM_PARAMS
	};

	enum InputIds 
	{
		IN_RUN_EXT_CV,
		IN_RESET_EXT_CV,
		IN_TEMPO_EXT_CV,
		IN_TIMESIGNATURETOP_EXT_CV,
		IN_TIMESIGNATUREBOTTOM_EXT_CV,
		IN_ROOT_KEY_EXT_CV,
		IN_SCALE_EXT_CV,
		IN_CLOCK_EXT_CV,
		IN_HARMONY_CIRCLE_DEGREE_EXT_CV,
		IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV,

		
		IN_MELODY_ENABLE_EXT_CV,			
		IN_MELODY_VOLUME_EXT_CV,
		IN_MELODY_DESTUTTER_EXT_CV,
		IN_MELODY_NOTE_LENGTH_DIVISOR_EXT_CV,
		IN_MELODY_TARGETOCTAVE_EXT_CV,
		IN_MELODY_ALPHA_EXT_CV,
		IN_MELODY_RANGE_EXT_CV,

		IN_ARP_ENABLE_EXT_CV,
		IN_ARP_COUNT_EXT_CV,
		IN_ARP_INCREMENT_EXT_CV,
		IN_ARP_DECAY_EXT_CV,
		IN_ARP_PATTERN_EXT_CV,
		IN_ENABLE_ARP_CHORDAL_EXT_CV,
		IN_ENABLE_ARP_SCALER_EXT_CV,


		IN_HARMONY_ENABLE_EXT_CV,
		IN_HARMONY_DESTUTTER_EXT_CV,
		IN_HARMONY_VOLUME_EXT_CV,
		IN_HARMONY_STEPS_EXT_CV,
		IN_HARMONY_TARGETOCTAVE_EXT_CV,
		IN_HARMONY_ALPHA_EXT_CV,
		IN_HARMONY_RANGE_EXT_CV,
		IN_HARMONY_DIVISOR_EXT_CV,
		IN_HARMONYPRESETS_EXT_CV,

		IN_BASS_ENABLE_EXT_CV,
		IN_BASS_VOLUME_EXT_CV,
		IN_BASS_TARGETOCTAVE_EXT_CV,
		IN_BASS_ACCENT_EXT_CV,
		IN_BASS_SYNCOPATE_EXT_CV,
		IN_BASS_SHUFFLE_EXT_CV,
		IN_BASS_DIVISOR_EXT_CV,

				
		IN_HARMONY_FBM_OCTAVES_EXT_CV,
		IN_MELODY_FBM_OCTAVES_EXT_CV,
		IN_ARP_FBM_OCTAVES_EXT_CV,

		IN_HARMONY_FBM_PERIOD_EXT_CV,
		IN_MELODY_FBM_PERIOD_EXT_CV,
		IN_ARP_FBM_PERIOD_EXT_CV,

		IN_ENABLE_MELODY_CHORDAL_EXT_CV,
		IN_MELODY_SCALER_EXT_CV,

		IN_ENABLE_HARMONY_ALL7THS_EXT_CV,
		IN_ENABLE_HARMONY_V7THS_EXT_CV,

		IN_ENABLE_HARMONY_STACCATO_EXT_CV,
		IN_ENABLE_MELODY_STACCATO_EXT_CV,
		IN_ENABLE_BASS_STACCATO_EXT_CV,

		IN_BASS_OCTAVES_EXT_CV,

		IN_PROG_STEP_EXT_CV,

		IN_MELODY_SCALE_DEGREE_EXT_CV,
		IN_MELODY_SCALE_GATE_EXT_CV,
				

		NUM_INPUTS
		
	};
	
	

	enum OutputIds 
	{
		OUT_RUN_OUT,
		OUT_RESET_OUT,
		OUT_TEMPO_OUT,
		OUT_CLOCK_OUT, 
		OUT_MELODY_GATE_OUTPUT,
		OUT_HARMONY_GATE_OUTPUT,
		OUT_HARMONY_GATE4_OUTPUT,
		OUT_BASS_GATE_OUTPUT,
		OUT_BASS_GATE2_OUTPUT,
		OUT_FBM_HARMONY_OUTPUT,
		OUT_MELODY_CV_OUTPUT,
		OUT_FBM_MELODY_OUTPUT,
		OUT_BASS_CV_OUTPUT,
		OUT_BASS_NOTE2_OUTPUT,
		OUT_HARMONY_CV_OUTPUT,
		OUT_HARMONY_NOTE2_OUTPUT,		
		OUT_HARMONY_NOTE3_OUTPUT,		
		OUT_HARMONY_NOTE4_OUTPUT,		
		OUT_CLOCK_BEATX2_OUTPUT,
		OUT_CLOCK_BAR_OUTPUT,
		OUT_CLOCK_BEATX4_OUTPUT,
		OUT_CLOCK_BEATX8_OUTPUT,
		OUT_CLOCK_BEAT_OUTPUT,
		// OUT_BASS_TRIGGER_OUTPUT,
		// OUT_HARMONY_TRIGGER_OUTPUT,
		// OUT_MELODY_TRIGGER_OUTPUT,
		OUT_FBM_ARP_OUTPUT,
		OUT_MELODY_VOLUME_OUTPUT,
		OUT_HARMONY_VOLUME_OUTPUT,
		OUT_BASS_VOLUME_OUTPUT,
		OUT_EXT_POLY_SCALE_OUTPUT,
	
		NUM_OUTPUTS
	};

	enum LightIds 
	{
		LIGHT_LEDBUTTON_HARMONY_ENABLE,
		LIGHT_LEDBUTTON_MELODY_ENABLE,
		LIGHT_LEDBUTTON_ARP_ENABLE,
		LIGHT_LEDBUTTON_ARP_ENABLE_CHORDAL,
		LIGHT_LEDBUTTON_ARP_ENABLE_SCALER,
		LIGHT_LEDBUTTON_BASS_ENABLE,
		LIGHT_LEDBUTTON_RUN,
		LIGHT_LEDBUTTON_RESET,
		LIGHT_LEDBUTTON_MELODY_DESTUTTER,
		LIGHT_LEDBUTTON_CIRCLESTEP_1,
		LIGHT_LEDBUTTON_CIRCLESTEP_2,
		LIGHT_LEDBUTTON_CIRCLESTEP_3,
		LIGHT_LEDBUTTON_CIRCLESTEP_4,
		LIGHT_LEDBUTTON_CIRCLESTEP_5,
		LIGHT_LEDBUTTON_CIRCLESTEP_6,
		LIGHT_LEDBUTTON_CIRCLESTEP_7,
		LIGHT_LEDBUTTON_CIRCLESTEP_8,
		LIGHT_LEDBUTTON_CIRCLESTEP_9,
		LIGHT_LEDBUTTON_CIRCLESTEP_10,
		LIGHT_LEDBUTTON_CIRCLESTEP_11,
		LIGHT_LEDBUTTON_CIRCLESTEP_12,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_2_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_3_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_4_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_5_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_6_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_7_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_8_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_9_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_10_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_11_LIGHT,
		LIGHT_CIRCLE_ROOT_KEY_POSITION_12_LIGHT,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_1,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_2,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_3,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_4,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_5,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_6,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_7,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_8,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_9,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_10,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_11,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_12,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_13,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_14,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_15,
		LIGHT_LEDBUTTON_CIRCLESETSTEP_16,
		LIGHT_CLOCK_IN_LIGHT,
		LIGHT_LEDBUTTON_MELODY_ENABLE_CHORDAL,
		LIGHT_LEDBUTTON_MELODY_ENABLE_SCALER,

		LIGHT_LEDBUTTON_ENABLE_HARMONY_ALL7THS_PARAM,
		LIGHT_LEDBUTTON_ENABLE_HARMONY_V7THS_PARAM,
		LIGHT_LEDBUTTON_BASS_SYNCOPATE_PARAM,
		LIGHT_LEDBUTTON_BASS_ACCENT_PARAM,
		LIGHT_LEDBUTTON_BASS_SHUFFLE_PARAM,

		LIGHT_LEDBUTTON_ENABLE_HARMONY_STACCATO_PARAM,
		LIGHT_LEDBUTTON_ENABLE_MELODY_STACCATO_PARAM,
		LIGHT_LEDBUTTON_ENABLE_BASS_STACCATO_PARAM,

		LIGHT_LEDBUTTON_BASS_OCTAVES_PARAM,

		LIGHT_LEDBUTTON_PROG_STEP_PARAM,
		
		NUM_LIGHTS
	};

	#include "data_and_functions.hpp"


	// Clock code adapted from Strum and AS

	struct LFOGenerator 
	{
		float phase = 0.0f;
		float pw = 0.5f;
		float freq = 1.0f;
		dsp::SchmittTrigger resetTrigger;
		void setFreq(float freq_to_set)
		{
			freq = freq_to_set;
		}		
		void step(float dt) 
		{
			float deltaPhase = fminf(freq * dt, 0.5f);
			phase += deltaPhase;
			if (phase >= 1.0f)
				phase -= 1.0f;
		}
		void setReset(float reset) 
		{
			if (resetTrigger.process(reset)) 
			{
				phase = 0.0f;
			}
		}
		float sqr() 
		{
			float sqr = (phase < pw) ? 1.0f : -1.0f;
			return sqr;
		}
	};  // struct LFOGenerator 

	void userPlaysCirclePositionHarmony(int circle_position, float octaveOffset)  // C=0   play immediate
	{
		if (doDebug) DEBUG("userPlaysCirclePositionHarmony(%d)", circle_position); 
		if (doDebug) DEBUG("circle_position=%d", circle_position);
	
		theMeanderState.last_harmony_chord_root_note=circle_of_fifths[circle_position];

		if (octaveOffset>9)
			octaveOffset=9;

		for (int i=0; i<7; ++i) // melody and bass will use this to accompany 
		{
			if  (theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].CircleIndex==circle_position)
			{
				int theDegree=theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].Degree;
				for (int j=0;j<MAX_STEPS;++j)
				{
					if (theHarmonyTypes[harmony_type].harmony_steps[j]==theDegree)
					{
						theMeanderState.last_harmony_step=j;
						break;
					}
				}
				break;
			}
		} 

		theMeanderState.theMelodyParms.last_step=theMeanderState.last_harmony_step;
		int note_index=	(int)(theMeanderState.theMelodyParms.note_avg*num_step_chord_notes[theMeanderState.last_harmony_step]);		// not sure this is necessary
		theMeanderState.theMelodyParms.last_chord_note_index= note_index;
		 
		int current_chord_note=0;
		int root_key_note=circle_of_fifths[circle_position]; 
		if (doDebug) DEBUG("root_key_note=%d %s", root_key_note, note_desig[root_key_note%12]); 
		int circle_chord_type= theCircleOf5ths.Circle5ths[circle_position].chordType;
		theMeanderState.theHarmonyParms.last_chord_type=circle_chord_type;
		if (doDebug) DEBUG("circle_chord_type=%d", circle_chord_type);
		int num_chord_members=chord_type_num_notes[circle_chord_type];
		if (doDebug) DEBUG("num_chord_members=%d", num_chord_members);

		if (((theMeanderState.theHarmonyParms.enable_all_7ths)||(theMeanderState.theHarmonyParms.enable_V_7ths))			
			&& ((circle_chord_type==2)
			||  (circle_chord_type==3)
			||  (circle_chord_type==4)
			||  (circle_chord_type==5)))
				outputs[OUT_HARMONY_CV_OUTPUT].setChannels(4);  // set polyphony
			else
				outputs[OUT_HARMONY_CV_OUTPUT].setChannels(3);  // set polyphony

		for (int j=0;j<num_chord_members;++j) 
		{
			current_chord_note=(int)((int)root_key_note+(int)chord_type_intervals[circle_chord_type][j]);
			if (doDebug) DEBUG("  current_chord_note=%d %s", current_chord_note, note_desig[current_chord_note%12]);
			int note_to_play=current_chord_note+(octaveOffset*12);
			outputs[OUT_HARMONY_CV_OUTPUT].setVoltage((note_to_play/12.0)-4.0,j);
			if (j>0&&j<4)
			{
				outputs[OUT_HARMONY_CV_OUTPUT+j].setVoltage((note_to_play/12.0)-4.0);
			}
					
			if (j<4)
			{
				theMeanderState.theHarmonyParms.last[j].note=note_to_play;
				theMeanderState.theHarmonyParms.last[j].noteType=NOTE_TYPE_CHORD;
				theMeanderState.theHarmonyParms.last[j].length=1;  // whole  note for now
				theMeanderState.theHarmonyParms.last[j].time32s=barts_count;
				theMeanderState.theHarmonyParms.last[j].countInBar=bar_note_count;
				if (bar_note_count<256)
				played_notes_circular_buffer[bar_note_count++]=theMeanderState.theHarmonyParms.last[j];
				
			}
            
		}
		float durationFactor=1.0;
		if (theMeanderState.theHarmonyParms.enable_staccato)
			durationFactor=0.5;
		else
			durationFactor=1.0;
		
		float note_duration=durationFactor*time_sig_top/(frequency*theMeanderState.theHarmonyParms.note_length_divisor);
		harmonyGatePulse.reset();  // kill the pulse in case it is active
		harmonyGatePulse.trigger(note_duration);  
	}

	void doHarmony(int barChordNumber=1, bool playFlag=false)
	{
		theMeanderState.updateDisplay = true;
		if (doDebug) DEBUG("doHarmony");
		if (doDebug) DEBUG("doHarmony() theActiveHarmonyType.min_steps=%d, theActiveHarmonyType.max_steps=%d", theActiveHarmonyType.min_steps, theActiveHarmonyType.max_steps );

		outputs[OUT_HARMONY_VOLUME_OUTPUT].setVoltage(theMeanderState.theHarmonyParms.volume);
		
		clock_t current_cpu_t= clock();  // cpu clock ticks since program began
		double current_cpu_time_double= (double)(current_cpu_t) / (double)CLOCKS_PER_SEC;
		
		if (doDebug) DEBUG("\nHarmony: barCount=%d Time=%.3lf", bar_count, (double)current_cpu_time_double);
													
		current_melody_note += 1.0/12.0;
		current_melody_note=fmod(current_melody_note, 1.0f);	

		
	
		for (int i=0; i<MAX_CIRCLE_STATIONS; ++i) {
			CircleStepStates[i] = false;
			lights[LIGHT_LEDBUTTON_CIRCLESTEP_1+i].setBrightness(0.0f);
		}

		for (int i=0; i<16; ++i) {
			if (i<theActiveHarmonyType.num_harmony_steps)
				lights[LIGHT_LEDBUTTON_CIRCLESETSTEP_1+i].setBrightness(0.25f);
			else
				lights[LIGHT_LEDBUTTON_CIRCLESETSTEP_1+i].setBrightness(0.0f);
				
		}
	
	
     	if (doDebug) DEBUG("theHarmonyTypes[%d].num_harmony_steps=%d", harmony_type, theActiveHarmonyType.num_harmony_steps);
		int step=(bar_count%theActiveHarmonyType.num_harmony_steps);  // 0-(n-1)
 
 		if ((harmony_type==22)&&(step==0)&&(barChordNumber==0))  // random coming home
		{
			float rnd = rack::random::uniform();
			int temp_num_harmony_steps=1 + (int)((rnd*(theHarmonyTypes[22].num_harmony_steps-1)));
			bar_count += (theHarmonyTypes[22].num_harmony_steps-temp_num_harmony_steps);
		}

		if (randomize_harmony) // this could be used to randomize any progression
		{
			if (barChordNumber==0)
			{
				float rnd = rack::random::uniform();
				step = (int)((rnd*theActiveHarmonyType.num_harmony_steps));
				step=step%theActiveHarmonyType.num_harmony_steps;
			}
			else
			{
				step=theMeanderState.theHarmonyParms.last_circle_step;
			}
		}
		else
		if (harmony_type==22) // random coming home
		{
			if (barChordNumber!=0)
			{
				step=theMeanderState.theHarmonyParms.last_circle_step;
			}
		}
		else
		if (harmony_type==23) // random order
		{
			if (barChordNumber==0)
			{
				float rnd = rack::random::uniform();
				step = (int)((rnd*theActiveHarmonyType.num_harmony_steps));
				step=step%theActiveHarmonyType.num_harmony_steps;
			}
			else
			{
				step=theMeanderState.theHarmonyParms.last_circle_step;
			}
		}
		else
		if ((harmony_type==31)||(harmony_type==42)||(harmony_type==43)||(harmony_type==44)||(harmony_type==45)||(harmony_type==46)||(harmony_type==47)||(harmony_type==48))  // Markov chains
		{   
			if (barChordNumber==0)
			{
				float rnd = rack::random::uniform();
				if (doDebug) DEBUG("rnd=%.2f",rnd);
			

				if (theMeanderState.theHarmonyParms.last_circle_step==-1)
				{
					step=0;
				}
				else
				{
					float probabilityTargetBottom[8]={0};  // skip first array index since this is 1 based
					float probabilityTargetTop[8]={0};     // skip first array index since this is 1 based
					float bottom=0;
					step=1;
					for (int i=1; i<8; ++i)  // skip first array index since this is 1 based
					{
						probabilityTargetBottom[i]=bottom;
						if (harmony_type==31)
							probabilityTargetTop[i]=bottom+MarkovProgressionTransitionMatrixBach1[theMeanderState.theHarmonyParms.last_circle_step+1][i];
						else
						if (harmony_type==42)
							probabilityTargetTop[i]=bottom+MarkovProgressionTransitionMatrixBach2[theMeanderState.theHarmonyParms.last_circle_step+1][i];
						else
						if (harmony_type==43)
							probabilityTargetTop[i]=bottom+MarkovProgressionTransitionMatrixMozart1[theMeanderState.theHarmonyParms.last_circle_step+1][i];
						else
						if (harmony_type==44)
							probabilityTargetTop[i]=bottom+MarkovProgressionTransitionMatrixMozart2[theMeanderState.theHarmonyParms.last_circle_step+1][i];
						else
						if (harmony_type==45)
							probabilityTargetTop[i]=bottom+MarkovProgressionTransitionMatrixPalestrina1[theMeanderState.theHarmonyParms.last_circle_step+1][i];
						else
						if (harmony_type==46)
							probabilityTargetTop[i]=bottom+MarkovProgressionTransitionMatrixBeethoven1[theMeanderState.theHarmonyParms.last_circle_step+1][i];
						else
						if (harmony_type==47)
							probabilityTargetTop[i]=bottom+MarkovProgressionTransitionMatrixTraditional1[theMeanderState.theHarmonyParms.last_circle_step+1][i];
						else
						if (harmony_type==48)
							probabilityTargetTop[i]=bottom+MarkovProgressionTransitionMatrix_I_IV_V[theMeanderState.theHarmonyParms.last_circle_step+1][i];
											
						
						bottom=probabilityTargetTop[i];
					}
					if (doDebug) DEBUG("Markov Probabilities:");
					for (int i=1; i<8; ++i)  // skip first array index since this is 1 based
					{
						if (harmony_type==31)
						{
							if (doDebug) DEBUG("i=%d: p=%.2f b=%.2f t=%.2f", i, MarkovProgressionTransitionMatrixBach1[theMeanderState.theHarmonyParms.last_circle_step+1][i], probabilityTargetBottom[i], probabilityTargetTop[i]);
						}
						else
						if (harmony_type==42)
						{
							if (doDebug) DEBUG("i=%d: p=%.2f b=%.2f t=%.2f", i, MarkovProgressionTransitionMatrixBach2[theMeanderState.theHarmonyParms.last_circle_step+1][i], probabilityTargetBottom[i], probabilityTargetTop[i]);
						}
						else
						if (harmony_type==43)
						{
							if (doDebug) DEBUG("i=%d: p=%.2f b=%.2f t=%.2f", i, MarkovProgressionTransitionMatrixMozart1[theMeanderState.theHarmonyParms.last_circle_step+1][i], probabilityTargetBottom[i], probabilityTargetTop[i]);
						}
						else
						if (harmony_type==44)
						{
							if (doDebug) DEBUG("i=%d: p=%.2f b=%.2f t=%.2f", i, MarkovProgressionTransitionMatrixMozart2[theMeanderState.theHarmonyParms.last_circle_step+1][i], probabilityTargetBottom[i], probabilityTargetTop[i]);
						}
						else
						if (harmony_type==45)
						{
							if (doDebug) DEBUG("i=%d: p=%.2f b=%.2f t=%.2f", i, MarkovProgressionTransitionMatrixPalestrina1[theMeanderState.theHarmonyParms.last_circle_step+1][i], probabilityTargetBottom[i], probabilityTargetTop[i]);
						}
						else
						if (harmony_type==46)
						{
							if (doDebug) DEBUG("i=%d: p=%.2f b=%.2f t=%.2f", i, MarkovProgressionTransitionMatrixBeethoven1[theMeanderState.theHarmonyParms.last_circle_step+1][i], probabilityTargetBottom[i], probabilityTargetTop[i]);
						}
						else
						if (harmony_type==47)
						{
							if (doDebug) DEBUG("i=%d: p=%.2f b=%.2f t=%.2f", i, MarkovProgressionTransitionMatrixTraditional1[theMeanderState.theHarmonyParms.last_circle_step+1][i], probabilityTargetBottom[i], probabilityTargetTop[i]);
						}
						else
						if (harmony_type==48)
						{
							if (doDebug) DEBUG("i=%d: p=%.2f b=%.2f t=%.2f", i, MarkovProgressionTransitionMatrix_I_IV_V[theMeanderState.theHarmonyParms.last_circle_step+1][i], probabilityTargetBottom[i], probabilityTargetTop[i]);
						}					

						if ((rnd>probabilityTargetBottom[i])&&(rnd<= probabilityTargetTop[i]))
						{
							step=i-1;
							if (doDebug) DEBUG("step=%d", step);
						}
					}
				
				}
			}
			else
			{
				step=theMeanderState.theHarmonyParms.last_circle_step;
			}
			
		}
		
    	

		if (doDebug) DEBUG("step=%d", step);

		int degreeStep=(theActiveHarmonyType.harmony_steps[step])%8;  
		if (doDebug) DEBUG("degreeStep=%d", degreeStep);
	
		theMeanderState.theHarmonyParms.last_circle_step=step;  // used for Markov chain

		//find this in semicircle
		for (int i=0; i<7; ++i)
		{
			if  (theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].Degree==degreeStep)
			{
				current_circle_position = theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].CircleIndex; 
				break;
			}
			if (i==7)
			{
	    	   if (doDebug) DEBUG("  warning circleposition could not be found 2");
			}
		}
				
		lights[LIGHT_LEDBUTTON_CIRCLESETSTEP_1+step].setBrightness(1.0f);
		lights[LIGHT_LEDBUTTON_CIRCLESTEP_1+ (current_circle_position)%12].setBrightness(1.0f);
		
		if (doDebug) DEBUG("current_circle_position=%d root=%d %s", current_circle_position, circle_of_fifths[current_circle_position], note_desig[circle_of_fifths[current_circle_position]]);		
		if (doDebug) DEBUG("theCircleOf5ths.Circle5ths[current_circle_position].chordType=%d", theCircleOf5ths.Circle5ths[current_circle_position].chordType);
		
		
		double period=1.0/theMeanderState.theHarmonyParms.period; // 1/seconds
		double fBmarg=theMeanderState.theHarmonyParms.seed + (double)(period*current_cpu_time_double); 
	    double fBmrand=(FastfBm1DNoise(fBmarg,theMeanderState.theHarmonyParms.noctaves) +1.)/2; 
			
		theMeanderState.theHarmonyParms.note_avg = 
			(1.0-theMeanderState.theHarmonyParms.alpha)*theMeanderState.theHarmonyParms.note_avg + 
			theMeanderState.theHarmonyParms.alpha*(theMeanderState.theHarmonyParms.range_bottom + (fBmrand*theMeanderState.theHarmonyParms.r1));
					
		if (theMeanderState.theHarmonyParms.note_avg>theMeanderState.theHarmonyParms.range_top)
		theMeanderState.theHarmonyParms.note_avg=theMeanderState.theHarmonyParms.range_top;
		if (theMeanderState.theHarmonyParms.note_avg<theMeanderState.theHarmonyParms.range_bottom)
		theMeanderState.theHarmonyParms.note_avg=theMeanderState.theHarmonyParms.range_bottom;
					
		int step_chord_type= theCircleOf5ths.Circle5ths[current_circle_position].chordType;
		
		if (((theMeanderState.theHarmonyParms.enable_all_7ths)||(theMeanderState.theHarmonyParms.enable_V_7ths))			
		&& ((theCircleOf5ths.Circle5ths[current_circle_position].chordType==2)
		||  (theCircleOf5ths.Circle5ths[current_circle_position].chordType==3)
		||  (theCircleOf5ths.Circle5ths[current_circle_position].chordType==4)
		||  (theCircleOf5ths.Circle5ths[current_circle_position].chordType==5)))
			outputs[OUT_HARMONY_CV_OUTPUT].setChannels(4);  // set polyphony
		else
			outputs[OUT_HARMONY_CV_OUTPUT].setChannels(3);  // set polyphony
		
		if (doDebug) DEBUG("step_chord_type=%d", step_chord_type);
		int num_chord_members=chord_type_num_notes[step_chord_type]; 
		if (doDebug) DEBUG("num_chord_members=%d", num_chord_members);
		
	
		theMeanderState.theHarmonyParms.last_chord_type=step_chord_type;
		theMeanderState.last_harmony_chord_root_note=circle_of_fifths[current_circle_position];
		theMeanderState.last_harmony_step=step;
	

		if (doDebug) DEBUG("theMeanderState.last_harmony_chord_root_note=%d %s", theMeanderState.last_harmony_chord_root_note, note_desig[theMeanderState.last_harmony_chord_root_note%MAX_NOTES]);

		if (doDebug) DEBUG("1st 3 step_chord_notes=%d %s, %d %s, %d %s", step_chord_notes[step][0], note_desig[step_chord_notes[step][0]%MAX_NOTES], step_chord_notes[step][1], note_desig[step_chord_notes[step][1]%MAX_NOTES], step_chord_notes[step][2], note_desig[step_chord_notes[step][2]%MAX_NOTES]);
			
		bool tonicFound=false;
		for (int j=0;j<num_chord_members;++j) 
		{
				if (doDebug) DEBUG("num_step_chord_notes[%d]=%d", step, num_step_chord_notes[step]);
				current_chord_notes[j]= step_chord_notes[step][(int)(theMeanderState.theHarmonyParms.note_avg*num_step_chord_notes[step])+j]; // may create inversion
				if (doDebug) DEBUG("current_chord_notes[%d]=%d %s", j, current_chord_notes[j], note_desig[current_chord_notes[j]%MAX_NOTES]);
						
				int note_to_play=current_chord_notes[j]-12;  // drop it an octave to get target octave right
				if (doDebug) DEBUG("    h_note_to_play=%d %s", note_to_play, note_desig[note_to_play%MAX_NOTES]);
							
				if (playFlag)  
				{
					if (j<4)
					{
						theMeanderState.theHarmonyParms.last[j].note=note_to_play;
						theMeanderState.theHarmonyParms.last[j].noteType=NOTE_TYPE_CHORD;
						theMeanderState.theHarmonyParms.last[j].length=1;  // need chords per measure
						theMeanderState.theHarmonyParms.last[j].time32s=barts_count;
						theMeanderState.theHarmonyParms.last[j].countInBar=bar_note_count;
						if (bar_note_count<256)
						played_notes_circular_buffer[bar_note_count++]=theMeanderState.theHarmonyParms.last[j];
					}
				
					//  get tonic in channel 0
											
					if ( (note_to_play%MAX_NOTES)==(theMeanderState.last_harmony_chord_root_note%MAX_NOTES))
					{
						outputs[OUT_HARMONY_CV_OUTPUT].setVoltage((note_to_play/12.0)-4.0,0);  // (note, channel)
						tonicFound=true;
					}
					else
					{
						if (!tonicFound) {
							outputs[OUT_HARMONY_CV_OUTPUT].setVoltage((note_to_play/12.0)-4.0,j+1);  // (note, channel)
							if (j<3)
								outputs[OUT_HARMONY_CV_OUTPUT+j+1].setVoltage((note_to_play/12.0)-4.0);
						} else {
							outputs[OUT_HARMONY_CV_OUTPUT].setVoltage((note_to_play/12.0)-4.0,j);  // (note, channel)
							if (j>0&&j<4)
								outputs[OUT_HARMONY_CV_OUTPUT+j].setVoltage((note_to_play/12.0)-4.0);
						}
					}
					
				
				}
		}
		
		if (playFlag)
		{ 
			outputs[OUT_HARMONY_VOLUME_OUTPUT].setVoltage(theMeanderState.theHarmonyParms.volume);

			// output some fBm noise
			// outputs[OUT_FBM_HARMONY_OUTPUT].setChannels(1);  // set polyphony  
		
			float durationFactor=1.0;
			if (theMeanderState.theHarmonyParms.enable_staccato)
				durationFactor=0.5;
			else
				durationFactor=0.95;

					
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BAR_OUTPUT)
					durationFactor*=1.0;
				else	
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BEAT_OUTPUT)
					durationFactor*=.25;
				else	
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BEATX2_OUTPUT)
					durationFactor*=.125;
				else	
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BEATX4_OUTPUT)
					durationFactor*=.0625;	
				else
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BEATX8_OUTPUT)
					durationFactor*=.03125;	
			else
			if ( inputs[IN_PROG_STEP_EXT_CV].isConnected()) // something is connected to the circle STEP input but we do not know what. Assume it is an 16X BPM frequency
			   		durationFactor *= .01562;  
			
						
			float note_duration=durationFactor*4/(frequency*theMeanderState.theHarmonyParms.note_length_divisor);
			harmonyGatePulse.reset();  // kill the pulse in case it is active
			harmonyGatePulse.trigger(note_duration);  
		}

		outputs[OUT_FBM_HARMONY_OUTPUT].setVoltage((float)clamp((10.f*fBmrand), 0.f, 10.f) ,0); // rescale fBm output to 0-10V so it can be used better for CV.  Output even if harmony disabled

		++circle_step_index;
		if (circle_step_index>=theActiveHarmonyType.num_harmony_steps)
			circle_step_index=0;

		if ((harmony_type==22)&&(step==0)&&(barChordNumber==0))  // random coming home
		{
			float rnd = rack::random::uniform();
			int temp_num_harmony_steps=1 + (int)((rnd*(theHarmonyTypes[22].num_harmony_steps-1)));
			bar_count += (theHarmonyTypes[22].num_harmony_steps-temp_num_harmony_steps);
		}

	}

	void userPlaysScaleDegreeMelody(int degree, float octaveOffset)  // C=0   play immediate
	{
		if (degree<1)
			degree=1;
		if (degree>7)
			degree=7;

		

		int	note_to_play=root_key_notes[root_key][degree-1]+(octaveOffset*12); 
		outputs[OUT_MELODY_CV_OUTPUT].setVoltage((note_to_play/12.0)-4.0,0);  // (note, channel)  shift down 1 ocatve/v
					
		theMeanderState.theMelodyParms.last[0].note=note_to_play;
		theMeanderState.theMelodyParms.last[0].noteType=NOTE_TYPE_MELODY;
		theMeanderState.theMelodyParms.last[0].length=1;  // whole  note for now
		theMeanderState.theMelodyParms.last[0].time32s=barts_count;
		theMeanderState.theMelodyParms.last[0].countInBar=bar_note_count;

		if (bar_note_count<256)
		played_notes_circular_buffer[bar_note_count++]=theMeanderState.theMelodyParms.last[0];
			
		float durationFactor=1.0;
		if (theMeanderState.theMelodyParms.enable_staccato)
			durationFactor=0.5;
		else
			durationFactor=1.0;
		
		float note_duration=durationFactor*time_sig_top/(frequency*theMeanderState.theMelodyParms.note_length_divisor);
		melodyGatePulse.reset();  // kill the pulse in case it is active
		melodyGatePulse.trigger(note_duration);  
	}

	void doMelody()
	{
		theMeanderState.updateDisplay = true;
		if (doDebug) DEBUG("doMelody()");

		outputs[OUT_MELODY_VOLUME_OUTPUT].setVoltage(theMeanderState.theMelodyParms.volume);
		clock_t current_cpu_t= clock();  // cpu clock ticks since program began
		double current_cpu_time_double= (double)(current_cpu_t) / (double)CLOCKS_PER_SEC;
	
		if (doDebug) DEBUG("Melody: Time=%.3lf",  (double)current_cpu_time_double);

		++theMeanderState.theMelodyParms.bar_melody_counted_note;

		theMeanderState.theArpParms.note_count=0;  // where does this really go, at the begining of a melody note
	
		double period=1.0/theMeanderState.theMelodyParms.period; // 1/seconds
		double fBmarg=theMeanderState.theMelodyParms.seed + (double)(period*current_cpu_time_double); 
		double fBmrand=(FastfBm1DNoise(fBmarg,theMeanderState.theMelodyParms.noctaves) +1.)/2; 
			
		theMeanderState.theMelodyParms.note_avg = 
			(1.0-theMeanderState.theMelodyParms.alpha)*theMeanderState.theMelodyParms.note_avg + 
			theMeanderState.theMelodyParms.alpha*(theMeanderState.theMelodyParms.range_bottom + (fBmrand*theMeanderState.theMelodyParms.r1));
					
		if (theMeanderState.theMelodyParms.note_avg>theMeanderState.theMelodyParms.range_top)
		theMeanderState.theMelodyParms.note_avg=theMeanderState.theMelodyParms.range_top;
		if (theMeanderState.theMelodyParms.note_avg<theMeanderState.theMelodyParms.range_bottom)
		theMeanderState.theMelodyParms.note_avg=theMeanderState.theMelodyParms.range_bottom;
		
		int step=theMeanderState.last_harmony_step;	
		theMeanderState.theMelodyParms.last_step= step;
		int note_index=	(int)(theMeanderState.theMelodyParms.note_avg*num_step_chord_notes[step]);		
		theMeanderState.theMelodyParms.last_chord_note_index= note_index;
		int note_to_play=step_chord_notes[step][note_index]; 

		if (theMeanderState.theMelodyParms.chordal)
		{
			note_index=	(int)(theMeanderState.theMelodyParms.note_avg*num_step_chord_notes[step]);	
			note_to_play=step_chord_notes[step][note_index]; 
	
		}
		else
		if (theMeanderState.theMelodyParms.scaler)
		{
			note_index=	(int)(theMeanderState.theMelodyParms.note_avg*num_root_key_notes[root_key]);
			note_to_play=root_key_notes[root_key][note_index]; 
		}


        if (false) // accidentals are probably not notated correctly  Flat accidentals may sound better than sharp
		{
			if ((theMeanderState.theMelodyParms.bar_melody_counted_note!=1)&&(theMeanderState.theMelodyParms.bar_melody_counted_note==(theMeanderState.theMelodyParms.note_length_divisor-1))) // allow accidentals, but not on first or last melody note in bar
			{
			
				float rnd = rack::random::uniform();
				if (rnd<.05)
					note_to_play += 1;
				else
				if (rnd>.90)
					note_to_play -= 1;
			}
		}
				
		if (doDebug) DEBUG("    melody note_to_play=%d %s", note_to_play, note_desig[note_to_play%MAX_NOTES]);

		if (true)	// do it even if melody notes will not be played, so arp will have roots
		{   
			if ((theMeanderState.theMelodyParms.destutter) && (note_to_play==theMeanderState.theMelodyParms.last[0].note) && (theMeanderState.theMelodyParms.last_stutter_step==step))   // seems like gate always fires
			{
				theMeanderState.theMelodyParms.stutter_detected=true;
				theMeanderState.theMelodyParms.last_stutter_step=step;
			}
			else
			{
				theMeanderState.theMelodyParms.last_stutter_step=step;
				theMeanderState.theMelodyParms.stutter_detected=false;
										
				theMeanderState.theMelodyParms.last[0].note=note_to_play;
				theMeanderState.theMelodyParms.last[0].noteType=NOTE_TYPE_MELODY;
				theMeanderState.theMelodyParms.last[0].length=theMeanderState.theMelodyParms.note_length_divisor;
				theMeanderState.theMelodyParms.last[0].time32s=barts_count;
				theMeanderState.theMelodyParms.last[0].countInBar=bar_note_count;

				if ((theMeanderState.theMelodyParms.enabled)&&(bar_note_count<256))
					played_notes_circular_buffer[bar_note_count++]=theMeanderState.theMelodyParms.last[0];
			

				if (theMeanderState.theMelodyParms.enabled)
				{						
					// outputs[OUT_MELODY_CV_OUTPUT].setChannels(1);  // set polyphony  may need to deal with unset channel voltages
					outputs[OUT_MELODY_CV_OUTPUT].setVoltage(((note_to_play/12.0) -4.0) ,0);  // (note, channel) -4 since midC=c4=0voutputs[OUT_MELODY_CV_OUTPUT].setVoltage((note_to_play -4 + theMeanderState.theMelodyParms.target_octave,0);  // (note, channel) -4 since midC=c4=0v
		    	}

				// output some fBm noise
				// outputs[OUT_FBM_MELODY_OUTPUT].setChannels(1);  // set polyphony  
				outputs[OUT_FBM_MELODY_OUTPUT].setVoltage((float)clamp((10.f*fBmrand), 0.f, 10.f) ,0); // rescale fBm output to 0-10V so it can be used better for CV

				float durationFactor=1.0;
				if (theMeanderState.theMelodyParms.enable_staccato)
					durationFactor=0.5;
				else
					durationFactor=0.95;
				float note_duration=durationFactor*4/(frequency*theMeanderState.theMelodyParms.note_length_divisor);

				// adjust melody note duration if arp enabled
				
				if (theMeanderState.theArpParms.enabled)
					note_duration=durationFactor*4/(frequency*theMeanderState.theArpParms.note_length_divisor);
				

				if (theMeanderState.theMelodyParms.enabled)
				melodyGatePulse.trigger(note_duration);  // Test 1s duration  need to use .process to detect this and then send it to output
			}
		}
	}

	void doArp() 
	{
		theMeanderState.updateDisplay = true;
		if (doDebug) DEBUG("doArp()");
	
	    if (theMeanderState.theArpParms.note_count>=theMeanderState.theArpParms.count)
	  		return;

		int arp_note=0;

		
		if ((theMeanderState.theArpParms.pattern>=-1)&&(theMeanderState.theArpParms.pattern<=1))
			arp_note=theMeanderState.theArpParms.note_count*theMeanderState.theArpParms.pattern;
		else
		if (theMeanderState.theArpParms.pattern==2)
		{
			if (theMeanderState.theArpParms.note_count<=((theMeanderState.theArpParms.count)/2))
			arp_note=theMeanderState.theArpParms.note_count;
			else
			arp_note=theMeanderState.theArpParms.count-theMeanderState.theArpParms.note_count-1;
		}
		else
		if (theMeanderState.theArpParms.pattern==-2)
		{
			if (theMeanderState.theArpParms.note_count<=((theMeanderState.theArpParms.count)/2))
			arp_note-=theMeanderState.theArpParms.note_count;
			else
			arp_note=-theMeanderState.theArpParms.count+theMeanderState.theArpParms.note_count-1;
		}
		else
			arp_note=theMeanderState.theArpParms.note_count*theMeanderState.theArpParms.pattern;

		if (theMeanderState.theArpParms.pattern!=0)
		++arp_note; // above the melody note

		++theMeanderState.theArpParms.note_count;

		float volume=theMeanderState.theMelodyParms.volume;
		float volume_factor=pow((1.0-theMeanderState.theArpParms.decay), theMeanderState.theArpParms.note_count);
		volume *= volume_factor;

		
		if (doDebug) DEBUG("theMeanderState.theMelodyParms.last_chord_note_index=%d", theMeanderState.theMelodyParms.last_chord_note_index);
		if (doDebug) DEBUG("num_step_chord_notes[%d]=%d", theMeanderState.theMelodyParms.last_step, num_step_chord_notes[theMeanderState.theMelodyParms.last_step]);
				
		int note_to_play=100; // bogus

		if (theMeanderState.theArpParms.chordal) // use step_chord_notes
		{
           note_to_play=step_chord_notes[theMeanderState.theMelodyParms.last_step][(theMeanderState.theMelodyParms.last_chord_note_index + arp_note)% num_step_chord_notes[theMeanderState.theMelodyParms.last_step]];
		}
		else 
		if (theMeanderState.theArpParms.scaler) // use root_key_notes rather than step_chord_notes.  This is slower since scale note index has to be looked up
		{   
			if (false)  // old brute force search from beginning
			{
				for (int x=0; x<num_root_key_notes[root_key]; ++x)
				{
					if (root_key_notes[root_key][x]==theMeanderState.theMelodyParms.last[0].note)
					{
						note_to_play=root_key_notes[root_key][x+arp_note];
						if (doDebug) DEBUG("note fount at index=%d root_key_notes[root_key][x]=%d", x, root_key_notes[root_key][x]);
						break;
					}
				}
			}
		
			if (true)  // new // BSP search  .  
			{
				int note_to_search_for=theMeanderState.theMelodyParms.last[0].note;
				if (doDebug) DEBUG("BSP  note_to_search_for=%d",  note_to_search_for);
				int num_to_search=num_root_key_notes[root_key];
				if (doDebug) DEBUG("BSP num_to_search=%d", num_to_search);
				int start_search_index=0;
				int end_search_index=num_root_key_notes[root_key]-1;
				int pass=0;
				int partition_index=0;
				while (pass<8)
				{
					if (doDebug) DEBUG("start_search_index=%d end_search_index=%d", start_search_index, end_search_index);
					partition_index=(end_search_index+start_search_index)/2;
					if (doDebug) DEBUG("BSP start_search_index=%d end_search_index=%d partition_index=%d", start_search_index, end_search_index, partition_index);
					if ( note_to_search_for>root_key_notes[root_key][partition_index])
					{
						start_search_index=partition_index;
						if (doDebug) DEBUG(">BSP root_key_notes[root_key][partition_index]=%d", root_key_notes[root_key][partition_index]);
					}
					else
					if ( note_to_search_for<root_key_notes[root_key][partition_index])
					{
						end_search_index=partition_index;
						if (doDebug) DEBUG("<BSP root_key_notes[root_key][partition_index]=%d", root_key_notes[root_key][partition_index]);
					}
					else
					{
						/* we found it */
						if (doDebug) DEBUG("value %d found at index %d", root_key_notes[root_key][partition_index], partition_index);
						pass=8;
						break;
					}
					++pass;
				}
				if ((partition_index>=0) && (partition_index<num_to_search))
					note_to_play=root_key_notes[root_key][partition_index+arp_note];
							
			}
			
		}
		
		
	
		if (((theMeanderState.theMelodyParms.enabled)||(theMeanderState.theArpParms.enabled))&&theMeanderState.theArpParms.note_count<32)
		{
			theMeanderState.theArpParms.last[theMeanderState.theArpParms.note_count].note=note_to_play;
			theMeanderState.theArpParms.last[theMeanderState.theArpParms.note_count].noteType=NOTE_TYPE_ARP;
			theMeanderState.theArpParms.last[theMeanderState.theArpParms.note_count].length=theMeanderState.theArpParms.note_length_divisor;
			theMeanderState.theArpParms.last[theMeanderState.theArpParms.note_count].time32s=barts_count;
			theMeanderState.theArpParms.last[theMeanderState.theArpParms.note_count].countInBar=bar_note_count;
			if (bar_note_count<256)
			played_notes_circular_buffer[bar_note_count++]=theMeanderState.theArpParms.last[theMeanderState.theArpParms.note_count];
		}
	
		// outputs[OUT_MELODY_CV_OUTPUT].setChannels(1);  // set polyphony  may need to deal with unset channel voltages
		outputs[OUT_MELODY_CV_OUTPUT].setVoltage(((note_to_play/12.0) -4.0) ,0);  // (note, channel) -4 since midC=c4=0voutputs[OUT_MELODY_CV_OUTPUT].setVoltage((note_to_play -4 + theMeanderState.theMelodyParms.target_octave,0);  // (note, channel) -4 since midC=c4=0v
		outputs[OUT_MELODY_VOLUME_OUTPUT].setVoltage(volume);  // this is strictly for arp decay
		melodyGatePulse.reset();  // kill the pulse in case it is active
		float durationFactor=1.0;
		if (theMeanderState.theMelodyParms.enable_staccato)
			durationFactor=0.5;
		else
			durationFactor=0.95;
		float note_duration=durationFactor*4/(frequency*theMeanderState.theArpParms.note_length_divisor);
		melodyGatePulse.trigger(note_duration);  
	}
 

	void doBass()
	{
		theMeanderState.updateDisplay = true;
		if (doDebug) DEBUG("doBass()");

	    outputs[OUT_BASS_VOLUME_OUTPUT].setVoltage(theMeanderState.theBassParms.volume);
				
		if (theMeanderState.theBassParms.enabled) 
		{
			++theMeanderState.theBassParms.bar_bass_counted_note;
			if ((theMeanderState.theBassParms.syncopate)&&(theMeanderState.theBassParms.bar_bass_counted_note==2))  // experimenting with bass patterns
			  return;
			if ((theMeanderState.theBassParms.shuffle)&&((theMeanderState.theBassParms.bar_bass_counted_note%3)==2))  // experimenting with bass patterns
			return;

			if (theMeanderState.theBassParms.octave_enabled)
				outputs[OUT_BASS_CV_OUTPUT].setChannels(2);  // set polyphony  may need to deal with unset channel voltages
			else
				outputs[OUT_BASS_CV_OUTPUT].setChannels(1);  // set polyphony  may need to deal with unset channel voltages
			if (doDebug) DEBUG("    bass note to play=%d %s", theMeanderState.last_harmony_chord_root_note, note_desig[theMeanderState.last_harmony_chord_root_note%MAX_NOTES]);
				
			theMeanderState.theBassParms.last[0].note=theMeanderState.last_harmony_chord_root_note+ (theMeanderState.theBassParms.target_octave*12);  
			theMeanderState.theBassParms.last[0].noteType=NOTE_TYPE_BASS;
			theMeanderState.theBassParms.last[0].length=1;  // need bass notes per measure
			theMeanderState.theBassParms.last[0].time32s=barts_count;
			theMeanderState.theBassParms.last[0].countInBar=bar_note_count;
			if (bar_note_count<256)
			played_notes_circular_buffer[bar_note_count++]=theMeanderState.theBassParms.last[0];

			outputs[OUT_BASS_CV_OUTPUT].setVoltage((theMeanderState.last_harmony_chord_root_note/12.0)-4.0 +theMeanderState.theBassParms.target_octave ,0);  //(note, channel)
				
			if (theMeanderState.theBassParms.octave_enabled)
			{
		
				theMeanderState.theBassParms.last[1].note=theMeanderState.theBassParms.last[0].note+12; 
				theMeanderState.theBassParms.last[1].noteType=NOTE_TYPE_BASS;
				theMeanderState.theBassParms.last[1].length=1;  // need bass notes per measure
				theMeanderState.theBassParms.last[1].time32s=barts_count;
				theMeanderState.theBassParms.last[1].countInBar=bar_note_count;
				if (bar_note_count<256)
				played_notes_circular_buffer[bar_note_count++]=theMeanderState.theBassParms.last[1];

				outputs[OUT_BASS_CV_OUTPUT].setVoltage((theMeanderState.last_harmony_chord_root_note/12.0)-3.0 +theMeanderState.theBassParms.target_octave ,1);
			 	outputs[OUT_BASS_NOTE2_OUTPUT].setVoltage((theMeanderState.last_harmony_chord_root_note/12.0)-3.0 +theMeanderState.theBassParms.target_octave);
			}

			if (!theMeanderState.theBassParms.accent)  // need to redo with new gate procedure
			{
				theMeanderState.theBassParms.note_accented=false; 
			}
			else
			{
				if (theMeanderState.theBassParms.bar_bass_counted_note==1)  // experimenting with bass patterns
					theMeanderState.theBassParms.note_accented=true; 
				else
					theMeanderState.theBassParms.note_accented=false; 
			}	
			
			float durationFactor=1.0;
			if (theMeanderState.theBassParms.enable_staccato)
				durationFactor=0.5;
			else
				durationFactor=0.95;
			
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BAR_OUTPUT)
					durationFactor*=1.0;
				else	
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BEAT_OUTPUT)
					durationFactor*=.25;
				else	
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BEATX2_OUTPUT)
					durationFactor*=.125;
				else	
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BEATX4_OUTPUT)
					durationFactor*=.0625;	
				else
				if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port==OUT_CLOCK_BEATX8_OUTPUT)
					durationFactor*=.03125;	
			else
			if ( inputs[IN_PROG_STEP_EXT_CV].isConnected()) // something is connected to the circle STEP input but we do not know what. Assume it is an 16X BPM frequency
			   		durationFactor *= .01562;
			
			float note_duration=durationFactor*time_sig_top/(frequency*theMeanderState.theBassParms.note_length_divisor);

		    bassGatePulse.trigger(note_duration);  
		}
	}
   
	LFOGenerator LFOclock;
	
	dsp::SchmittTrigger ST_32ts_trig;  // 32nd note timer tick

	dsp::SchmittTrigger run_button_trig;
	dsp::SchmittTrigger ext_run_trig;
	dsp::SchmittTrigger reset_btn_trig;
	dsp::SchmittTrigger reset_ext_trig;
	dsp::SchmittTrigger bpm_mode_trig;
	dsp::SchmittTrigger step_button_trig;

	dsp::PulseGenerator resetPulse;
	bool reset_pulse = false;

	dsp::PulseGenerator runPulse;
	bool run_pulse = false;

	dsp::PulseGenerator stepPulse;
	bool step_pulse = false;

	// PULSES FOR TRIGGER OUTPUTS INSTEAD OF GATES
	dsp::PulseGenerator clockPulse32ts;
	bool pulse32ts = false;
	dsp::PulseGenerator clockPulse16ts;
	bool pulse16ts = false;
	dsp::PulseGenerator clockPulse8ts;
	bool pulse8ts = false; 
	dsp::PulseGenerator clockPulse4ts;
	bool pulse4ts = false;
	dsp::PulseGenerator clockPulse2ts;
	bool pulse2ts = false;
	dsp::PulseGenerator clockPulse1ts;
	bool pulse1ts = false;
	
	float trigger_length = 0.0001f;

	const float lightLambda = 0.075f;
	float resetLight = 0.0f;
	float stepLight = 0.0f;

	bool running = true;
	
	int bar_count = 0;  // number of bars running count
	
	int i16ts_count = 0;  // counted 32s notes per sixteenth note
	int i8ts_count = 0;  // counted 32s notes per eighth note
	int i4ts_count = 0; // counted 32s notes per quarter note
	int i2ts_count = 0; // counted 32s notes per half note
	int barts_count = 0;     // counted 32s notes per bar

	float tempo =120.0f;
	float frequency = 2.0f;

	
	int i16ts_count_limit = 2;// 32s notes per sixteenth note
	int i8ts_count_limit = 4;   // 32s notes per eighth note
	int i4ts_count_limit = 8;  // 32s notes per quarter note
	int i2ts_count_limit =16;  // 32s notes per half note
	int barts_count_limit = 32;     // 32s notes per bar
	
	float min_bpm = 10.0f;
	float max_bpm = 300.0f;

	float extHarmonyIn=-99;
  

	// end of clock **************************

	dsp::ClockDivider lowFreqClock;
	
	float phase = 0.f;
  		
	float last_melody_note=0;
	float current_melody_note=0;

	int circle_step_index=0;

	dsp::SchmittTrigger HarmonyEnableToggle;
	dsp::SchmittTrigger MelodyEnableToggle;
	dsp::SchmittTrigger BassEnableToggle;
	dsp::SchmittTrigger ArpEnableToggle;
	dsp::SchmittTrigger ArpEnableChordalToggle;
	dsp::SchmittTrigger ArpEnableScalerToggle;

	dsp::SchmittTrigger HarmonyEnableAll7thsToggle;
	dsp::SchmittTrigger HarmonyEnableV7thsToggle;

	dsp::SchmittTrigger HarmonyEnableStaccatoToggle;
	 
	
	dsp::SchmittTrigger MelodyDestutterToggle;
	dsp::SchmittTrigger MelodyEnableChordalToggle;
	dsp::SchmittTrigger MelodyEnableScalerToggle;

	dsp::SchmittTrigger MelodyEnableStaccatoToggle;
	
	dsp::SchmittTrigger BassSyncopateToggle;
	dsp::SchmittTrigger BassAccentToggle;
	dsp::SchmittTrigger BassShuffleToggle;
	dsp::SchmittTrigger BassOctavesToggle;

	dsp::SchmittTrigger BassEnableStaccatoToggle;
		
	dsp::SchmittTrigger RunToggle;
		
	
	dsp::SchmittTrigger CircleStepToggles[MAX_STEPS];
	dsp::SchmittTrigger CircleStepSetToggles[MAX_STEPS];
	bool CircleStepStates[MAX_STEPS]={};
	bool CircleStepSetStates[MAX_STEPS]={};

	rack::dsp::PulseGenerator barTriggerPulse; 

	rack::dsp::PulseGenerator harmonyGatePulse; 
	rack::dsp::PulseGenerator melodyGatePulse; 
	rack::dsp::PulseGenerator bassGatePulse; 
	rack::dsp::PulseGenerator barGaterPulse; 

	bool time_sig_changed=false;

	int override_step=1;

    // save button states
	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// running
			// running
		json_object_set_new(rootJ, "running", json_boolean(running)); 

		json_object_set_new(rootJ, "theHarmonyParmsenabled", json_boolean(theMeanderState.theHarmonyParms.enabled));
		json_object_set_new(rootJ, "harmony_staccato_enable", json_boolean(theMeanderState.theHarmonyParms.enable_staccato));
		json_object_set_new(rootJ, "theHarmonyParmsenable_all_7ths", json_boolean(theMeanderState.theHarmonyParms.enable_all_7ths));
		json_object_set_new(rootJ, "theHarmonyParmsenable_V_7ths", json_boolean(theMeanderState.theHarmonyParms.enable_V_7ths));
		json_object_set_new(rootJ, "theMelodyParmsenabled", json_boolean(theMeanderState.theMelodyParms.enabled));
		json_object_set_new(rootJ, "theMelodyParmsdestutter", json_boolean(theMeanderState.theMelodyParms.destutter));
		json_object_set_new(rootJ, "theMelodyParmsenable_staccato", json_boolean(theMeanderState.theMelodyParms.enable_staccato));
		json_object_set_new(rootJ, "theMelodyParmschordal", json_boolean(theMeanderState.theMelodyParms.chordal));
		json_object_set_new(rootJ, "theMelodyParmsscaler", json_boolean(theMeanderState.theMelodyParms.scaler));
		json_object_set_new(rootJ, "theArpParmsenabled", json_boolean(theMeanderState.theArpParms.enabled));
		json_object_set_new(rootJ, "theArpParmschordal", json_boolean(theMeanderState.theArpParms.chordal));
		json_object_set_new(rootJ, "theArpParmsscaler", json_boolean(theMeanderState.theArpParms.scaler));
		json_object_set_new(rootJ, "theBassParmsenabled", json_boolean(theMeanderState.theBassParms.enabled));
		json_object_set_new(rootJ, "theBassParmsenable_staccato", json_boolean(theMeanderState.theBassParms.enable_staccato));
		json_object_set_new(rootJ, "theBassParmssyncopate", json_boolean(theMeanderState.theBassParms.syncopate));
		json_object_set_new(rootJ, "theBassParmsaccent", json_boolean(theMeanderState.theBassParms.accent));
		json_object_set_new(rootJ, "theBassParmsshuffle", json_boolean(theMeanderState.theBassParms.shuffle));
		json_object_set_new(rootJ, "theBassParmsoctave_enabled", json_boolean(theMeanderState.theBassParms.octave_enabled));
		
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// running
		json_t *runningJ = json_object_get(rootJ, "running");
		if (runningJ)
			running = json_is_true(runningJ);
				
		json_t *HarmonyParmsenabledJ = json_object_get(rootJ, "theHarmonyParmsenabled");
		if (HarmonyParmsenabledJ)
			theMeanderState.theHarmonyParms.enabled = json_is_true(HarmonyParmsenabledJ);

		json_t *HarmonyParmsstaccato_enableJ = json_object_get(rootJ, "harmony_staccato_enable");
		if (HarmonyParmsstaccato_enableJ)
			theMeanderState.theHarmonyParms.enable_staccato = json_is_true(HarmonyParmsstaccato_enableJ);

		json_t *HarmonyParmsenable_all_7thsJ = json_object_get(rootJ, "theHarmonyParmsenable_all_7ths");
		if (HarmonyParmsenable_all_7thsJ)
			theMeanderState.theHarmonyParms.enable_all_7ths = json_is_true(HarmonyParmsenable_all_7thsJ);

		json_t *HarmonyParmsenable_V_7thsJ = json_object_get(rootJ, "theHarmonyParmsenable_V_7ths");
		if (HarmonyParmsenable_V_7thsJ)
			theMeanderState.theHarmonyParms.enable_V_7ths = json_is_true(HarmonyParmsenable_V_7thsJ);

		json_t *MelodyParmsenabledJ = json_object_get(rootJ, "theMelodyParmsenabled");
		if (MelodyParmsenabledJ)
			theMeanderState.theMelodyParms.enabled = json_is_true(MelodyParmsenabledJ);

		json_t *MelodyParmsdestutterJ = json_object_get(rootJ, "theMelodyParmsdestutter");
		if (MelodyParmsdestutterJ)
			theMeanderState.theMelodyParms.destutter = json_is_true(MelodyParmsdestutterJ);
			
		json_t *MelodyParmsenable_staccatoJ = json_object_get(rootJ, "theMelodyParmsenable_staccato");
		if (MelodyParmsenable_staccatoJ)
			theMeanderState.theMelodyParms.enable_staccato = json_is_true(MelodyParmsenable_staccatoJ);

		json_t *MelodyParmschordalJ = json_object_get(rootJ, "theMelodyParmschordal");
		if (MelodyParmschordalJ)
			theMeanderState.theMelodyParms.chordal = json_is_true(MelodyParmschordalJ);

		json_t *MelodyParmsscalerJ = json_object_get(rootJ, "theMelodyParmsscaler");
		if (MelodyParmsscalerJ)
			theMeanderState.theMelodyParms.scaler = json_is_true(MelodyParmsscalerJ);

		json_t *ArpParmsenabledJ = json_object_get(rootJ, "theArpParmsenabled");
		if (ArpParmsenabledJ)
			theMeanderState.theArpParms.enabled = json_is_true(ArpParmsenabledJ);

		json_t *ArpParmschordalJ = json_object_get(rootJ, "theArpParmschordal");
		if (ArpParmschordalJ)
			theMeanderState.theArpParms.chordal = json_is_true(ArpParmschordalJ);

		json_t *ArpParmsscalerJ = json_object_get(rootJ, "theArpParmsscaler");
		if (ArpParmsscalerJ)
			theMeanderState.theArpParms.scaler = json_is_true(ArpParmsscalerJ);

		json_t *BassParmsenabledJ = json_object_get(rootJ, "theBassParmsenabled");
		if (BassParmsenabledJ)
			theMeanderState.theBassParms.enabled = json_is_true(BassParmsenabledJ);

		json_t *BassParmsenable_staccatoJ = json_object_get(rootJ, "theBassParmsenable_staccato");
		if (BassParmsenable_staccatoJ)
			theMeanderState.theBassParms.enable_staccato = json_is_true(BassParmsenable_staccatoJ);

		json_t *BassParmssyncopateJ = json_object_get(rootJ, "theBassParmssyncopate");
		if (BassParmssyncopateJ)
			theMeanderState.theBassParms.syncopate = json_is_true(BassParmssyncopateJ);

		json_t *BassParmsaccentJ = json_object_get(rootJ, "theBassParmsaccent");
		if (BassParmsaccentJ)
			theMeanderState.theBassParms.accent = json_is_true(BassParmsaccentJ);

		json_t *BassParmsshuffleJ = json_object_get(rootJ, "theBassParmsshuffle");
		if (BassParmsshuffleJ)
			theMeanderState.theBassParms.shuffle = json_is_true(BassParmsshuffleJ);

		json_t *BassParmsoctave_enabledJ = json_object_get(rootJ, "theBassParmsoctave_enabled");
		if (BassParmsoctave_enabledJ)
			theMeanderState.theBassParms.octave_enabled = json_is_true(BassParmsoctave_enabledJ);

		// Make sure savedHarmonySteps is reset by always applying it after load
		circleChanged = true;
		savedHarmonySteps = params[CONTROL_HARMONY_STEPS_PARAM].getValue();
	}

	    	
	void process(const ProcessArgs &args) override 
	{
		
		if (!instanceRunning)
			return;
	
		if (!globalsInitialized)
			return;

		//Run
	
		if (RunToggle.process(params[BUTTON_RUN_PARAM].getValue() || inputs[IN_RUN_EXT_CV].getVoltage()))  
		{ 
			running=!running;

			if(!running)
			{
				i16ts_count = 0;  
				i8ts_count = 0;  
				i4ts_count = 0; 
				i2ts_count = 0; 
				barts_count = 0;    
				
				theMeanderState.theMelodyParms.bar_melody_counted_note=0;
				theMeanderState.theArpParms.note_count=0;
				theMeanderState.theBassParms.bar_bass_counted_note=0;
				outputs[OUT_CLOCK_BAR_OUTPUT].setVoltage(0.0f);	   // bars 	
				outputs[OUT_CLOCK_BEAT_OUTPUT].setVoltage(0.0f);   // 4ts 
				outputs[OUT_CLOCK_BEATX2_OUTPUT].setVoltage(0.0f); // 8ts
				outputs[OUT_CLOCK_BEATX4_OUTPUT].setVoltage(0.0f); // 16ts
				outputs[OUT_CLOCK_BEATX8_OUTPUT].setVoltage(0.0f); // 32ts
			}
			else
			{
				LFOclock.setFreq(frequency*(32/time_sig_bottom));	  // for 32ts	
				barts_count_limit = (32*time_sig_top/time_sig_bottom);
			}
			theMeanderState.theHarmonyParms.pending_step_edit=0;
			runPulse.trigger(0.01f); // delay 10ms
		}
		lights[LIGHT_LEDBUTTON_RUN].setBrightness(running ? 1.0f : 0.0f); 
		run_pulse = runPulse.process(1.0 / args.sampleRate);  
		outputs[OUT_RUN_OUT].setVoltage((run_pulse ? 10.0f : 0.0f));

		if (inputs[IN_TEMPO_EXT_CV].isConnected())
		{
			float fvalue=inputs[IN_TEMPO_EXT_CV].getVoltage();
			tempo=std::round(std::pow(2.0, fvalue)*120);
			if (tempo<10)
				tempo=10;
			if (tempo>300)
				tempo=300;

			if (true)  // adjust the tempo knob and param
			{
				params[CONTROL_TEMPOBPM_PARAM].setValue(tempo);
			}
		}
		else
		{
			float fvalue = std::round(params[CONTROL_TEMPOBPM_PARAM].getValue());
			if (fvalue!=tempo)
			tempo=fvalue;
		}

		frequency = tempo/60.0f;  // drives 1 tick per 32nd note
						
		// Reset

		
		if (reset_btn_trig.process(params[BUTTON_RESET_PARAM].getValue() || inputs[IN_RESET_EXT_CV].getVoltage() || time_sig_changed)) 
		{
		//	setup_harmony();
			time_sig_changed=false;
	    	LFOclock.setReset(1.0f);
			bar_count = 0;
			bar_note_count=0;
			i16ts_count = 0;  
			i8ts_count = 0;  
			i4ts_count = 0; 
			i2ts_count = 0; 
			barts_count = 0;    
			 

			theMeanderState.theMelodyParms.bar_melody_counted_note=0;
			theMeanderState.theArpParms.note_count=0;
			theMeanderState.theBassParms.bar_bass_counted_note=0;

			theMeanderState.theHarmonyParms.last_circle_step=-1; // for Markov chain
			
			resetLight = 1.0;
			resetPulse.trigger(0.01f);  // necessary to pass on reset below vis resetPuls.process()

				
			if (!running)
			{
				harmonyGatePulse.reset();  // kill the pulse in case it is active
				melodyGatePulse.reset();  // kill the pulse in case it is active
				bassGatePulse.reset();  // kill the pulse in case it is active
				outputs[OUT_HARMONY_GATE_OUTPUT].setVoltage(0);
				outputs[OUT_HARMONY_GATE4_OUTPUT].setVoltage(0);
				outputs[OUT_MELODY_GATE_OUTPUT].setVoltage(0);
				outputs[OUT_BASS_GATE_OUTPUT].setVoltage(0);
				outputs[OUT_BASS_GATE2_OUTPUT].setVoltage(0);
				outputs[OUT_HARMONY_VOLUME_OUTPUT].setVoltage(0);
				outputs[OUT_MELODY_VOLUME_OUTPUT].setVoltage(0);
				outputs[OUT_BASS_VOLUME_OUTPUT].setVoltage(0);
			}
			
		}

		resetLight -= resetLight / lightLambda / args.sampleRate;
		lights[LIGHT_LEDBUTTON_RESET].setBrightness(resetLight); 
		reset_pulse = resetPulse.process(1.0 / args.sampleRate);
  		outputs[OUT_RESET_OUT].setVoltage((reset_pulse ? 10.0f : 0.0f));
        
	
		if ((step_button_trig.process(params[BUTTON_PROG_STEP_PARAM].getValue() || (  inputs[IN_PROG_STEP_EXT_CV].isConnected()  &&  (inputs[IN_PROG_STEP_EXT_CV].getVoltage() > 0.))))) 
		{
			++bar_count;

			if (theMeanderState.theHarmonyParms.enabled)
			{
				theMeanderState.theHarmonyParms.enabled = false;
				override_step=0;
			}
			else
			{
				++override_step;
				if (override_step>=theActiveHarmonyType.num_harmony_steps)
				  override_step=0;
			    
			}
			theMeanderState.userControllingHarmonyFromCircle=true;
			theMeanderState.last_harmony_step=override_step;
		
			int current_circle_position=0;

			int degreeStep=(theActiveHarmonyType.harmony_steps[override_step])%8;  
			
			//find this in semicircle
			for (int j=0; j<7; ++j)
			{
				if  (theCircleOf5ths.theDegreeSemiCircle.degreeElements[j].Degree==degreeStep)
				{
					current_circle_position = theCircleOf5ths.theDegreeSemiCircle.degreeElements[j].CircleIndex; 
					if (doDebug) DEBUG("harmony step edit-pt2 current_circle_position=%d", current_circle_position);
					break;
				}
			}
		
			stepLight = 1.0;
			stepPulse.trigger(0.01f);  // necessary to pass on reset below vis resetPuls.process()
           			
			if (running)
			{
				doHarmony(0, true);
				if (theMeanderState.theBassParms.enabled)
					doBass();
			}
		
		}

		stepLight -= stepLight / lightLambda / args.sampleRate;
		lights[LIGHT_LEDBUTTON_PROG_STEP_PARAM].setBrightness(stepLight);
		step_pulse = stepPulse.process(1.0 / args.sampleRate);

		if(running)  
		{
			// these should be done in initialization rather than every process() call
			LFOclock.setFreq(frequency*(32/time_sig_bottom));	  // for 32ts	should not hurt top call this each sample
			barts_count_limit = (32*time_sig_top/time_sig_bottom);
			//************************************************************************
						 
			LFOclock.step(1.0 / args.sampleRate);

			bool clockTick=false;
			if ( inputs[IN_CLOCK_EXT_CV].isConnected())  // external clock connected to Clock input
			{
				if (!inportStates[IN_CLOCK_EXT_CV].inTransition)
				{
					if (ST_32ts_trig.process(inputs[IN_CLOCK_EXT_CV].getVoltage()))  // triggers from each external clock tick ONLY once when input reaches 1.0V
					{
						clockTick=true;
						// outputs[OUT_CLOCK_OUT].setChannels(1);  // set polyphony  
						outputs[OUT_CLOCK_OUT].setVoltage(10.0f);
						inportStates[IN_CLOCK_EXT_CV].inTransition=true;
					}
				}

				if (inportStates[IN_CLOCK_EXT_CV].inTransition)
				{
					if (ST_32ts_trig.process(math::rescale(inputs[IN_CLOCK_EXT_CV].getVoltage(),10.f,0.f,0.f,10.f)))  // triggers from each external clock tick ONLY once when inverted input reaches 0.0V
					{
						// outputs[OUT_CLOCK_OUT].setChannels(1);  // set polyphony  
						outputs[OUT_CLOCK_OUT].setVoltage(0.0f);  
						inportStates[IN_CLOCK_EXT_CV].inTransition=false;
					}
				}
			}
			else // no external clock connected to Clock input, use internal clock
			{
				float IntClockLevel=5.0f*(LFOclock.sqr()+1.0f);
				if (ST_32ts_trig.process(LFOclock.sqr()))                         // triggers from each external clock tick ONLY once when .sqr() reaches 1.0V
				{
					 clockTick=true;
				}
			
				 // outputs[OUT_CLOCK_OUT].setChannels(1);  // set polyphony  
				 outputs[OUT_CLOCK_OUT].setVoltage(IntClockLevel);  
			}
				
		    if (clockTick)
			{
				bool melodyPlayed=false;   // set to prevent arp note being played on the melody beat
				int barChordNumber=(int)((int)(barts_count*theMeanderState.theHarmonyParms.note_length_divisor)/(int)32);
			
				// bar
				if (barts_count == 0) 
				{
					theMeanderState.theMelodyParms.bar_melody_counted_note=0;
					theMeanderState.theBassParms.bar_bass_counted_note=0;
					bar_note_count=0;
					if ((theMeanderState.theHarmonyParms.note_length_divisor==1)&&(!theMeanderState.userControllingHarmonyFromCircle))
						doHarmony(barChordNumber, theMeanderState.theHarmonyParms.enabled);
					if ((theMeanderState.theBassParms.note_length_divisor==1)&&(!theMeanderState.userControllingHarmonyFromCircle))
						doBass();
					if ((theMeanderState.theMelodyParms.note_length_divisor==1)&&(!theMeanderState.userControllingMelody))
					{
						doMelody();
						melodyPlayed=true;
					}
					clockPulse1ts.trigger(trigger_length);
					// Pulse the output gate 
					barTriggerPulse.trigger(1e-3f);  // 1ms duration  need to use .process to detect this and then send it to output
				}
						
		        // i2ts
				if (i2ts_count == 0)
				{
					if ((theMeanderState.theHarmonyParms.note_length_divisor==2)&&(!theMeanderState.userControllingHarmonyFromCircle))
						doHarmony(barChordNumber, theMeanderState.theHarmonyParms.enabled);
					if ((theMeanderState.theBassParms.note_length_divisor==2)&&(!theMeanderState.userControllingHarmonyFromCircle))
						doBass();
					if ((theMeanderState.theMelodyParms.note_length_divisor==2)&&(!theMeanderState.userControllingMelody))
					{
						doMelody();
						melodyPlayed=true;
					}
					i2ts_count++;

					if (!theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port)
						clockPulse2ts.trigger(trigger_length);
				}
				else
				if (i2ts_count == (i2ts_count_limit-1))
				{
					i2ts_count = 0;    
					if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port)
						clockPulse2ts.trigger(trigger_length);
				}
				else
				{
					i2ts_count++;
				}
								
		
				// i4ts
				if (i4ts_count == 0)
				{
					if ((theMeanderState.theHarmonyParms.note_length_divisor==4)&&(!theMeanderState.userControllingHarmonyFromCircle))
						doHarmony(barChordNumber, theMeanderState.theHarmonyParms.enabled);
					if ((theMeanderState.theBassParms.note_length_divisor==4)&&(!theMeanderState.userControllingHarmonyFromCircle))
						doBass();
					if ((theMeanderState.theMelodyParms.note_length_divisor==4)&&(!theMeanderState.userControllingMelody))
					{
						doMelody();
						melodyPlayed=true;
					}
					if ((theMeanderState.theArpParms.enabled)&&(theMeanderState.theArpParms.note_length_divisor==4)&&(!melodyPlayed))
						doArp();

					i4ts_count++;

					if (!theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port)
					 	clockPulse4ts.trigger(trigger_length);
				}
				else
				if (i4ts_count == (i4ts_count_limit-1))
				{
					i4ts_count = 0;  
					if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port)
					 	clockPulse4ts.trigger(trigger_length);
				}
				else
				{
					i4ts_count++;
				}
				
					  
		 		// i8ts
				if (i8ts_count == 0)
				{
					if ((theMeanderState.theHarmonyParms.note_length_divisor==8)&&(!theMeanderState.userControllingHarmonyFromCircle))
						doHarmony(barChordNumber, theMeanderState.theHarmonyParms.enabled);
					if ((theMeanderState.theBassParms.note_length_divisor==8)&&(!theMeanderState.userControllingHarmonyFromCircle))
						doBass();
					if ((theMeanderState.theMelodyParms.note_length_divisor==8)&&(!theMeanderState.userControllingMelody))
					{
						doMelody();
						melodyPlayed=true;
					}
					if ((theMeanderState.theArpParms.enabled)&&(theMeanderState.theArpParms.note_length_divisor==8)&&(!melodyPlayed))
						doArp();

					i8ts_count++;

					if (!theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port)
						clockPulse8ts.trigger(trigger_length);
				}
				else
				if (i8ts_count == (i8ts_count_limit-1))
				{
					i8ts_count = 0; 
					if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port)
					  clockPulse8ts.trigger(trigger_length);
				}
				else
				{
					i8ts_count++; 
				}
				
				
				// i16ts
				if (i16ts_count == 0)
				{
					if ((theMeanderState.theMelodyParms.note_length_divisor==16)&&(!theMeanderState.userControllingMelody))
					{
						doMelody();  
						melodyPlayed=true;  
					}
					if ((theMeanderState.theArpParms.enabled)&&(theMeanderState.theArpParms.note_length_divisor==16)&&(!melodyPlayed))
						doArp();

					i16ts_count++;

					if (!theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port)
						clockPulse16ts.trigger(trigger_length);
				}
				else
				if (i16ts_count == (i16ts_count_limit-1))
				{
					i16ts_count = 0;
					if (theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port)
						clockPulse16ts.trigger(trigger_length);
				}
				else
				{
					i16ts_count++;
				}
				

				//32nds  ***********************************

				clock_t current_cpu_t= clock();  // cpu clock ticks since program began
				double current_cpu_time_double= (double)(current_cpu_t) / (double)CLOCKS_PER_SEC;
			
				 // do on each 1/32nd clock tick
				
				if ((theMeanderState.theMelodyParms.note_length_divisor==32)&&(!theMeanderState.userControllingMelody))
				{
					doMelody();   
					melodyPlayed=true; 
				}
				if ((theMeanderState.theArpParms.enabled)&&(theMeanderState.theArpParms.note_length_divisor==32)&&(!melodyPlayed))
					doArp(); 
							
				// output some fBm noise
				double period=1.0/theMeanderState.theArpParms.period; // 1/seconds
				double fBmarg=theMeanderState.theArpParms.seed + (double)(period*current_cpu_time_double); 
				double fBmrand=(FastfBm1DNoise(fBmarg,theMeanderState.theArpParms.noctaves) +1.)/2; 
				// outputs[OUT_FBM_ARP_OUTPUT].setChannels(1);  // set polyphony  
				outputs[OUT_FBM_ARP_OUTPUT].setVoltage((float)clamp((10.f*fBmrand), 0.f, 10.f) ,0);  // rescale fBm output to 0-10V so it can be used better for CV
				
				if (barts_count == (barts_count_limit-1))  // do this after all processing so bar_count does not get incremented too early
				{
					barts_count = 0;  
					theMeanderState.theMelodyParms.bar_melody_counted_note=0;
					theMeanderState.theBassParms.bar_bass_counted_note=0;
					bar_note_count=0;
					if (!theMeanderState.userControllingHarmonyFromCircle)  // don't mess up bar count
					++bar_count; 
				}
				else
				{
					barts_count++;  
				}
				
				clockPulse32ts.trigger(trigger_length);  // retrigger the pulse after all done in this loop

			//	outputs[OUT_CLOCK_OUT].setChannels(1);  // set polyphony  
			//	outputs[OUT_CLOCK_OUT].setVoltage(10.0f);  
				
			}
			else  // !clockTick
			{
			//	outputs[OUT_CLOCK_OUT].setChannels(1);  // set polyphony  
			//	outputs[OUT_CLOCK_OUT].setVoltage(0.0f);  
			}
			
		}

		pulse1ts = clockPulse1ts.process(1.0 / args.sampleRate);
		pulse2ts = clockPulse2ts.process(1.0 / args.sampleRate);
		pulse4ts = clockPulse4ts.process(1.0 / args.sampleRate);
		pulse8ts = clockPulse8ts.process(1.0 / args.sampleRate);
		pulse16ts = clockPulse16ts.process(1.0 / args.sampleRate);
		pulse32ts = clockPulse32ts.process(1.0 / args.sampleRate);

		// end the gate if pulse timer has expired 

		if (true) // standard gate voltages 
		{
			outputs[OUT_HARMONY_GATE_OUTPUT].setVoltage( harmonyGatePulse.process( 1.0 / APP->engine->getSampleRate() ) ? CV_MAX10 : 0.0 ); 
			outputs[OUT_MELODY_GATE_OUTPUT].setVoltage( melodyGatePulse.process( 1.0 / APP->engine->getSampleRate() ) ? CV_MAX10 : 0.0 ); 
			outputs[OUT_BASS_GATE_OUTPUT].setVoltage( bassGatePulse.process( 1.0 / APP->engine->getSampleRate() ) ? CV_MAX10 : 0.0 ); 

			float bassVolumeLevel=theMeanderState.theBassParms.volume;
			if (theMeanderState.theBassParms.accent)
			{
				if (theMeanderState.theBassParms.note_accented)
					bassVolumeLevel=theMeanderState.theBassParms.volume;
				else
					bassVolumeLevel=0.8*theMeanderState.theBassParms.volume;
				bassVolumeLevel=clamp(bassVolumeLevel, 0.0f, 10.f); 

			}
			outputs[OUT_BASS_VOLUME_OUTPUT].setVoltage(bassVolumeLevel);
		}
		else  // non-standard volume over gate voltages
		{
			float harmonyGateLevel=theMeanderState.theHarmonyParms.volume; 
			harmonyGateLevel=clamp(harmonyGateLevel, 2.1f, 10.f);  // don't let gate on level drop below 2.0v so it will trigger ADSR etc.
			outputs[OUT_HARMONY_GATE_OUTPUT].setVoltage( harmonyGatePulse.process( 1.0 / APP->engine->getSampleRate() ) ? harmonyGateLevel : 0.0 ); 

			float melodyGateLevel=theMeanderState.theMelodyParms.volume; 
			melodyGateLevel=clamp(melodyGateLevel, 2.1f, 10.f);   // don't let gate on level drop below 2.0v so it will trigger ADSR etc.
			outputs[OUT_MELODY_GATE_OUTPUT].setVoltage( melodyGatePulse.process( 1.0 / APP->engine->getSampleRate() ) ? melodyGateLevel : 0.0 ); 

			float bassGateLevel=theMeanderState.theBassParms.volume;

			if (theMeanderState.theBassParms.accent)
			{
				if (!theMeanderState.theBassParms.note_accented)
					bassGateLevel*=.8;
			}

			bassGateLevel=clamp(bassGateLevel, 2.1f, 10.f); // don't let gate on level drop below 2.0v so it will trigger ADSR etc.
			outputs[OUT_BASS_GATE_OUTPUT].setVoltage( bassGatePulse.process( 1.0 / APP->engine->getSampleRate() ) ?bassGateLevel : 0.0 ); 
		}
		outputs[OUT_BASS_GATE2_OUTPUT].setVoltage(theMeanderState.theBassParms.octave_enabled ? outputs[OUT_BASS_GATE_OUTPUT].value : 0);
		outputs[OUT_HARMONY_GATE4_OUTPUT].setVoltage((theMeanderState.theHarmonyParms.last_chord_type==2)||(theMeanderState.theHarmonyParms.last_chord_type==3)||(theMeanderState.theHarmonyParms.last_chord_type==4)||(theMeanderState.theHarmonyParms.last_chord_type==5) ? outputs[OUT_HARMONY_GATE_OUTPUT].value : 0);
				
				
		outputs[OUT_CLOCK_BAR_OUTPUT].setVoltage((pulse1ts ? 10.0f : 0.0f));     // barts  
		outputs[OUT_CLOCK_BEAT_OUTPUT].setVoltage((pulse4ts ? 10.0f : 0.0f));    // 4ts
		outputs[OUT_CLOCK_BEATX2_OUTPUT].setVoltage((pulse8ts ? 10.0f : 0.0f));  // 8ts
		outputs[OUT_CLOCK_BEATX4_OUTPUT].setVoltage((pulse16ts ? 10.0f : 0.0f)); // 16ts
		outputs[OUT_CLOCK_BEATX8_OUTPUT].setVoltage((pulse32ts ? 10.0f : 0.0f)); // 32ts

	        
		if (HarmonyEnableToggle.process(params[BUTTON_ENABLE_HARMONY_PARAM].getValue())) 
		{
			theMeanderState.theHarmonyParms.enabled = !theMeanderState.theHarmonyParms.enabled;
			theMeanderState.userControllingHarmonyFromCircle=false;
		}
		lights[LIGHT_LEDBUTTON_HARMONY_ENABLE].setBrightness(theMeanderState.theHarmonyParms.enabled ? 1.0f : 0.0f); 

		if (HarmonyEnableAll7thsToggle.process(params[BUTTON_ENABLE_HARMONY_ALL7THS_PARAM].getValue())) 
		{
			theMeanderState.theHarmonyParms.enable_all_7ths = !theMeanderState.theHarmonyParms.enable_all_7ths;
			if (theMeanderState.theHarmonyParms.enable_all_7ths)
				theMeanderState.theHarmonyParms.enable_V_7ths=false;
			setup_harmony();  // calculate harmony notes
			circleChanged=true;
		}
		lights[LIGHT_LEDBUTTON_ENABLE_HARMONY_ALL7THS_PARAM].setBrightness(theMeanderState.theHarmonyParms.enable_all_7ths ? 1.0f : 0.0f); 
		

		if (HarmonyEnableV7thsToggle.process(params[BUTTON_ENABLE_HARMONY_V7THS_PARAM].getValue())) 
		{
			theMeanderState.theHarmonyParms.enable_V_7ths = !theMeanderState.theHarmonyParms.enable_V_7ths;
			if (theMeanderState.theHarmonyParms.enable_V_7ths)
				theMeanderState.theHarmonyParms.enable_all_7ths=false;
			setup_harmony();
			circleChanged=true;
		}
		lights[LIGHT_LEDBUTTON_ENABLE_HARMONY_V7THS_PARAM].setBrightness(theMeanderState.theHarmonyParms.enable_V_7ths ? 1.0f : 0.0f); 
//


		if (HarmonyEnableStaccatoToggle.process(params[BUTTON_ENABLE_HARMONY_STACCATO_PARAM].getValue())) 
		{		
			theMeanderState.theHarmonyParms.enable_staccato = !theMeanderState.theHarmonyParms.enable_staccato;
			
		}
		lights[LIGHT_LEDBUTTON_ENABLE_HARMONY_STACCATO_PARAM].setBrightness(theMeanderState.theHarmonyParms.enable_staccato); 
	

		if (MelodyEnableStaccatoToggle.process(params[BUTTON_ENABLE_MELODY_STACCATO_PARAM].getValue())) 
		{
			theMeanderState.theMelodyParms.enable_staccato = !theMeanderState.theMelodyParms.enable_staccato;
	
		}
		lights[LIGHT_LEDBUTTON_ENABLE_MELODY_STACCATO_PARAM].setBrightness(theMeanderState.theMelodyParms.enable_staccato ? 1.0f : 0.0f); 

		if (BassEnableStaccatoToggle.process(params[BUTTON_ENABLE_BASS_STACCATO_PARAM].getValue())) 
		{
			theMeanderState.theBassParms.enable_staccato = !theMeanderState.theBassParms.enable_staccato;
	
		}
		lights[LIGHT_LEDBUTTON_ENABLE_BASS_STACCATO_PARAM].setBrightness(theMeanderState.theBassParms.enable_staccato ? 1.0f : 0.0f); 
//
		
		if (BassEnableToggle.process(params[BUTTON_ENABLE_BASS_PARAM].getValue())) 
		{
			theMeanderState.theBassParms.enabled = !theMeanderState.theBassParms.enabled;
		}
		lights[LIGHT_LEDBUTTON_BASS_ENABLE].setBrightness(theMeanderState.theBassParms.enabled ? 1.0f : 0.0f); 

		
		
		if (MelodyEnableToggle.process(params[BUTTON_ENABLE_MELODY_PARAM].getValue())) 
		{
			theMeanderState.theMelodyParms.enabled = !theMeanderState.theMelodyParms.enabled;
			if (theMeanderState.theMelodyParms.enabled)
			   theMeanderState.userControllingMelody=false;
		}
		lights[LIGHT_LEDBUTTON_MELODY_ENABLE].setBrightness(theMeanderState.theMelodyParms.enabled ? 1.0f : 0.0f); 

		if (MelodyDestutterToggle.process(params[BUTTON_MELODY_DESTUTTER_PARAM].getValue())) 
		{
			theMeanderState.theMelodyParms.destutter = !theMeanderState.theMelodyParms.destutter;
		}
		lights[LIGHT_LEDBUTTON_MELODY_DESTUTTER].setBrightness(theMeanderState.theMelodyParms.destutter ? 1.0f : 0.0f); 

		if (MelodyEnableChordalToggle.process(params[BUTTON_ENABLE_MELODY_CHORDAL_PARAM].getValue())) 
		{
			theMeanderState.theMelodyParms.chordal = !theMeanderState.theMelodyParms.chordal;
			theMeanderState.theMelodyParms.scaler = !theMeanderState.theMelodyParms.scaler;
		}
		lights[LIGHT_LEDBUTTON_MELODY_ENABLE_CHORDAL].setBrightness(theMeanderState.theMelodyParms.chordal ? 1.0f : 0.0f); 

		if (MelodyEnableScalerToggle.process(params[BUTTON_ENABLE_MELODY_SCALER_PARAM].getValue())) 
		{
			theMeanderState.theMelodyParms.scaler = !theMeanderState.theMelodyParms.scaler;
			theMeanderState.theMelodyParms.chordal = !theMeanderState.theMelodyParms.chordal;
		}
		lights[LIGHT_LEDBUTTON_MELODY_ENABLE_SCALER].setBrightness(theMeanderState.theMelodyParms.scaler ? 1.0f : 0.0f); 

				

		if (ArpEnableToggle.process(params[BUTTON_ENABLE_ARP_PARAM].getValue())) 
		{
			theMeanderState.theArpParms.enabled = !theMeanderState.theArpParms.enabled;
		}
		lights[LIGHT_LEDBUTTON_ARP_ENABLE].setBrightness(theMeanderState.theArpParms.enabled ? 1.0f : 0.0f); 

		if (ArpEnableChordalToggle.process(params[BUTTON_ENABLE_ARP_CHORDAL_PARAM].getValue())) 
		{
			theMeanderState.theArpParms.chordal = !theMeanderState.theArpParms.chordal;
			theMeanderState.theArpParms.scaler = !theMeanderState.theArpParms.scaler;
		}
		lights[LIGHT_LEDBUTTON_ARP_ENABLE_CHORDAL].setBrightness(theMeanderState.theArpParms.chordal ? 1.0f : 0.0f); 

		if (ArpEnableScalerToggle.process(params[BUTTON_ENABLE_ARP_SCALER_PARAM].getValue())) 
		{
			theMeanderState.theArpParms.scaler = !theMeanderState.theArpParms.scaler;
			theMeanderState.theArpParms.chordal = !theMeanderState.theArpParms.chordal;
		}
		lights[LIGHT_LEDBUTTON_ARP_ENABLE_SCALER].setBrightness(theMeanderState.theArpParms.scaler ? 1.0f : 0.0f); 

		//****Bass

		if (BassSyncopateToggle.process(params[BUTTON_BASS_SYNCOPATE_PARAM].getValue())) 
		{
			theMeanderState.theBassParms.syncopate = !theMeanderState.theBassParms.syncopate;
		}
		lights[LIGHT_LEDBUTTON_BASS_SYNCOPATE_PARAM].setBrightness(theMeanderState.theBassParms.syncopate ? 1.0f : 0.0f); 	

		if (BassAccentToggle.process(params[BUTTON_BASS_ACCENT_PARAM].getValue())) 
		{
			theMeanderState.theBassParms.accent = !theMeanderState.theBassParms.accent;
		}
		lights[LIGHT_LEDBUTTON_BASS_ACCENT_PARAM].setBrightness(theMeanderState.theBassParms.accent ? 1.0f : 0.0f); 	

		if (BassShuffleToggle.process(params[BUTTON_BASS_SHUFFLE_PARAM].getValue())) 
		{
			theMeanderState.theBassParms.shuffle = !theMeanderState.theBassParms.shuffle;
		}
		lights[LIGHT_LEDBUTTON_BASS_SHUFFLE_PARAM].setBrightness(theMeanderState.theBassParms.shuffle ? 1.0f : 0.0f); 	

		if (BassOctavesToggle.process(params[BUTTON_BASS_OCTAVES_PARAM].getValue())) 
		{
			theMeanderState.theBassParms.octave_enabled = !theMeanderState.theBassParms.octave_enabled;
		}
		lights[LIGHT_LEDBUTTON_BASS_OCTAVES_PARAM].setBrightness(theMeanderState.theBassParms.octave_enabled ? 1.0f : 0.0f); 	
		
		//***************
			 
		

		for (int i=0; i<12; ++i) 
		{
			if (CircleStepToggles[i].process(params[BUTTON_CIRCLESTEP_C_PARAM+i].getValue()))  // circle button clicked
			{
				int current_circle_position=i;
				if (doDebug) DEBUG("harmony step edit-pt3 current_circle_position=%d", current_circle_position);

				for (int j=0; j<12; ++j) 
				{
					if (j!=current_circle_position) 
					{
						CircleStepStates[j] = false;
						lights[LIGHT_LEDBUTTON_CIRCLESTEP_1+j].setBrightness(CircleStepStates[j] ? 1.0f : 0.0f);
					}
				}

				CircleStepStates[current_circle_position] = !CircleStepStates[current_circle_position];
				lights[LIGHT_LEDBUTTON_CIRCLESTEP_1+current_circle_position].setBrightness(CircleStepStates[current_circle_position] ? 1.0f : 0.0f);	
			
				userPlaysCirclePositionHarmony(current_circle_position, theMeanderState.theHarmonyParms.target_octave); 
										
				theMeanderState.userControllingHarmonyFromCircle=true;
				theMeanderState.theHarmonyParms.enabled=false;
				lights[LIGHT_LEDBUTTON_HARMONY_ENABLE].setBrightness(theMeanderState.theHarmonyParms.enabled ? 1.0f : 0.0f); 
			
			
				//find this in circle
				
				for (int j=0; j<7; ++j) 
				{
					if  (theCircleOf5ths.theDegreeSemiCircle.degreeElements[j].CircleIndex==current_circle_position)
					{
						int theDegree=theCircleOf5ths.theDegreeSemiCircle.degreeElements[j].Degree;
						if (doDebug) DEBUG("harmony step edit-pt4 theDegree=%d", theDegree);
						if ((theDegree>=1)&&(theDegree<=7))
						{
							if (theMeanderState.theHarmonyParms.pending_step_edit)
							{
								if (doDebug) DEBUG("harmony step edit-pt5 theMeanderState.theHarmonyParms.pending_step_edit=%d", theMeanderState.theHarmonyParms.pending_step_edit);
								if (doDebug) DEBUG("harmony step edit-pt6 theDegree=%d found", theDegree);
								theHarmonyTypes[harmony_type].harmony_steps[theMeanderState.theHarmonyParms.pending_step_edit-BUTTON_HARMONY_SETSTEP_1_PARAM]=theDegree;
								//
								strcpy(theHarmonyTypes[harmony_type].harmony_degrees_desc,"");
								for (int k=0;k<theHarmonyTypes[harmony_type].num_harmony_steps;++k)
								{
									strcat(theHarmonyTypes[harmony_type].harmony_degrees_desc,circle_of_fifths_arabic_degrees[theHarmonyTypes[harmony_type].harmony_steps[k]]);  
									strcat(theHarmonyTypes[harmony_type].harmony_degrees_desc," ");
								}
								//
								copyHarmonyTypeToActiveHarmonyType(harmony_type);
								setup_harmony();
							}
						}
						break;
					}
				} 
			}
		}
			
		if (!running)
		{
			for (int i=0; i<theActiveHarmonyType.num_harmony_steps; ++i) 
			{
				if (CircleStepSetToggles[i].process(params[BUTTON_HARMONY_SETSTEP_1_PARAM+i].getValue())) 
				{
					if (doDebug) DEBUG("harmony step edit-pt1 step=%d clicked", i);
					int selectedStep=i;
					theMeanderState.theHarmonyParms.pending_step_edit=BUTTON_HARMONY_SETSTEP_1_PARAM+selectedStep;

					int current_circle_position=0;
					if (true)
					{
						int degreeStep=(theActiveHarmonyType.harmony_steps[selectedStep])%8;  
						
						//find this in semicircle
						for (int j=0; j<7; ++j)
						{
							if  (theCircleOf5ths.theDegreeSemiCircle.degreeElements[j].Degree==degreeStep)
							{
								current_circle_position = theCircleOf5ths.theDegreeSemiCircle.degreeElements[j].CircleIndex; 
								if (doDebug) DEBUG("harmony step edit-pt2 current_circle_position=%d", current_circle_position);
								break;
							}
						}
					}

					
					for (int i=0; i<12; ++i)  
					{
						lights[LIGHT_LEDBUTTON_CIRCLESTEP_1+i].setBrightness(0.0f);	
					}
					lights[LIGHT_LEDBUTTON_CIRCLESTEP_1+current_circle_position].setBrightness(1.0f);
										
					userPlaysCirclePositionHarmony(current_circle_position, theMeanderState.theHarmonyParms.target_octave);  
				
					CircleStepSetStates[i] = !CircleStepSetStates[i];
					lights[LIGHT_LEDBUTTON_CIRCLESETSTEP_1+i].setBrightness(CircleStepSetStates[i] ? 1.0f : 0.25f);
					
					for (int j=0; j<theActiveHarmonyType.num_harmony_steps; ++j) {
						if (j!=i) {
							CircleStepSetStates[j] = false;
							lights[LIGHT_LEDBUTTON_CIRCLESETSTEP_1+j].setBrightness(0.25f);
						}
					}

					
				}
			} 
		}

		float fvalue=0;
        float circleDegree=0;  // for harmony
		float scaleDegree=0;   // for melody
		float gateValue=0;

		if (  (inputs[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].isConnected()) && (inputs[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].isConnected()) )
		{
			circleDegree=inputs[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].getVoltage();
			gateValue=inputs[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].getVoltage(); 

			theMeanderState.theHarmonyParms.lastCircleDegreeIn=circleDegree;
			extHarmonyIn=circleDegree;
		
			float octave=(float)((int)(circleDegree));  // from the keyboard
			if (octave>3)
				octave=3;
			if (octave<-3)
				octave=-3;
			bool degreeChanged=false; // assume false unless determined true below
			bool skipStep=false;

			if ((gateValue==circleDegree)&&(circleDegree>=1)&&(circleDegree<=7.7))  // MarkovSeq or other 1-7V degree  degree.octave 0.0-7.7V
			{
				if (inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].inTransition)
				{
					if (circleDegree==inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue)
					{
						// was in transition but now is not
						inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].inTransition=false;
						inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue=circleDegree;
						octave = (int)(10.0*std::fmod(circleDegree, 1.0f));
						if (octave>7)
							octave=7;
						circleDegree=(float)((int)circleDegree);
						theMeanderState.circleDegree=(int)circleDegree;
						degreeChanged=true;
					}
					else
					if (circleDegree!=inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue)
					{
						inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue=circleDegree;
					}
				}
				else
				{
					if (circleDegree!=inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue)
					{
						inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].inTransition=true;
						inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue=circleDegree;
					}
				}

				if (circleDegree==0)
				{
					degreeChanged=false;
				}
			}
			else
			if ((gateValue==circleDegree)&&((circleDegree<1.0)||(circleDegree>=8.0)))  // MarkovSeq or other 1-7V degree  degree.octave 1.0-7.7V  <1 or >=8V means skip step
			{
				inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue=fvalue;
				degreeChanged=true;
				skipStep=true;
			}
			else  // keyboard  C-B
			{
					float fgvalue=inputs[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].getVoltage();
					if (inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].inTransition)
					{
						if (fgvalue==inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].lastValue)
						{
							// was in transition but now is not
							inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].inTransition=false;
							inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].lastValue=fgvalue;
							if (fgvalue)  // gate has gone high
							{
								if ( circleDegree==inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue) // the gate has changed but the degree has not
									degreeChanged=true;   // not really, but play like it has so it will be replayed below
							}
						}
						else
						if (fgvalue!=inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].lastValue)
						{
							inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].lastValue=fgvalue;
						}
					}
					else
					{
						if (fgvalue!=inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].lastValue)
						{
							inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].inTransition=true;
							inportStates[IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV].lastValue=fgvalue;
						}
					}

					if ( (degreeChanged) || (circleDegree!=inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue))
					{
						inportStates[IN_HARMONY_CIRCLE_DEGREE_EXT_CV].lastValue=circleDegree;
						if (circleDegree>=0)
							circleDegree=(float)std::fmod(std::fabs(circleDegree), 1.0f);
						else
							circleDegree=-(float)std::fmod(std::fabs(circleDegree), 1.0f);
						degreeChanged=true; 
						if (doDebug) DEBUG("IN_HARMONY_CIRCLE_DEGREE_EXT_CV circleDegree=%f", circleDegree);
						if (circleDegree>=0)
						{
							if ((std::abs(circleDegree)<.005f))  	   theMeanderState.circleDegree=1;
							else
							if ((std::abs(circleDegree-.167f)<.005f))  theMeanderState.circleDegree=2;
							else
							if ((std::abs(circleDegree-.333f)<.005f))  theMeanderState.circleDegree=3;
							else
							if ((std::abs(circleDegree-.417f)<.005f))  theMeanderState.circleDegree=4;
							else
							if ((std::abs(circleDegree-.583f)<.005f))  theMeanderState.circleDegree=5;
							else
							if ((std::abs(circleDegree-.750f)<.005f))  theMeanderState.circleDegree=6;
							else
							if ((std::abs(circleDegree-.917f)<.005f))  theMeanderState.circleDegree=7;
							else
								degreeChanged=false;
						}
						else
						{
							octave-=1;
							if ((std::abs(circleDegree)<.005f))  			 theMeanderState.circleDegree=1;
							else
							if (std::abs(std::abs(circleDegree)-.083)<.005f)  theMeanderState.circleDegree=7;
							else
							if (std::abs(std::abs(circleDegree)-.250)<.005f)  theMeanderState.circleDegree=6;
							else
							if (std::abs(std::abs(circleDegree)-.417)<.005f)  theMeanderState.circleDegree=5;
							else
							if (std::abs(std::abs(circleDegree)-.583)<.005f)  theMeanderState.circleDegree=4;
							else
							if (std::abs(std::abs(circleDegree)-.667)<.005f)  theMeanderState.circleDegree=3;
							else
							if (std::abs(std::abs(circleDegree)-.833)<.005f)  theMeanderState.circleDegree=2;
							else
								degreeChanged=false;
						}
						
					}	
				
			}
			
        	if ((degreeChanged)&&(!skipStep))
			{
				if (theMeanderState.circleDegree<1)
					theMeanderState.circleDegree=1;
				if (theMeanderState.circleDegree>7)
					theMeanderState.circleDegree=7;
				

				if (doDebug) DEBUG("IN_HARMONY_CIRCLE_DEGREE_EXT_CV=%d", (int)theMeanderState.circleDegree);
			//	DEBUG("IN_HARMONY_CIRCLE_DEGREE_EXT_CV=%d", (int)theMeanderState.circleDegree);

				int step=1;  // default if not found below
				for (int i=0; i<MAX_STEPS; ++i)
				{
					if (theActiveHarmonyType.harmony_steps[i]==theMeanderState.circleDegree)
					{
						step=i;
						break;
					}
				}

				theMeanderState.last_harmony_step=step;

				int theCirclePosition=0;
				for (int i=0; i<7; ++i)
				{
					if (theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].Degree==theMeanderState.circleDegree)
					{
						theCirclePosition=theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].CircleIndex;
						break;
					}
				}

				last_circle_position=theCirclePosition;
			
				userPlaysCirclePositionHarmony(theCirclePosition, octave+theMeanderState.theHarmonyParms.target_octave);  // play immediate
				if (doDebug) DEBUG("userPlaysCirclePositionHarmony()");
				if (theMeanderState.theBassParms.enabled)
			    	doBass();
			
				if (running)
				{
					theMeanderState.userControllingHarmonyFromCircle=true;
					theMeanderState.theHarmonyParms.enabled=false;
				}

				for (int i=0; i<12; ++i) 
				{
					CircleStepStates[i] = false;
					lights[LIGHT_LEDBUTTON_CIRCLESTEP_1+i].setBrightness(CircleStepStates[i] ? 1.0f : 0.0f);	
				}
			
				lights[LIGHT_LEDBUTTON_CIRCLESTEP_1+theCirclePosition].setBrightness(1.0f);
			}
			
		}
		

		//**************************			
		if (lowFreqClock.process())
		{
			if (!instanceRunning)
				return;
			// check controls for changes
		
			if ((fvalue=std::round(params[CONTROL_TEMPOBPM_PARAM].getValue()))!=tempo)
			{
				tempo = fvalue;
				if (doDebug) DEBUG("tempo changed to %d", (int)tempo);
			}
			
       		int ivalue=std::round(params[CONTROL_TIMESIGNATURETOP_PARAM].getValue());
			if (ivalue!=time_sig_top)
			{
				time_sig_top = ivalue;
				time_sig_changed=true;
			}	
			ivalue=std::round(params[CONTROL_TIMESIGNATUREBOTTOM_PARAM].getValue());
			if (std::pow(2,ivalue+1)!=time_sig_bottom)
			{
				time_sig_bottom = std::pow(2,ivalue+1);
				
				int melody_note_length_divisor=0;
				if (time_sig_bottom==2)
				  melody_note_length_divisor=1;
				else
				if (time_sig_bottom==4)
				  melody_note_length_divisor=2;
				else
				if (time_sig_bottom==8)
				  melody_note_length_divisor=3;
				else
				if (time_sig_bottom==16)
				  melody_note_length_divisor=4; 
				theMeanderState.theMelodyParms.note_length_divisor=(int)std::pow(2,melody_note_length_divisor);
				params[CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM].setValue(melody_note_length_divisor);

				theMeanderState.theArpParms.note_length_divisor=(int)std::pow(2,melody_note_length_divisor+1);
				params[CONTROL_ARP_INCREMENT_PARAM].setValue(melody_note_length_divisor+1);
				time_sig_changed=true;
			}
			
			
			frequency = tempo/60.0f;  // BPS
			
		
			if ((fvalue=std::round(params[CONTROL_ROOT_KEY_PARAM].getValue()))!=circle_root_key)
			{
				circle_root_key=(int)fvalue;
				root_key=circle_of_fifths[circle_root_key];
				if (doDebug) DEBUG("root_key changed to %d = %s", root_key, note_desig[root_key]);
				for (int i=0; i<12; ++i)
					lights[LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT+i].setBrightness(0.0f);
				lights[LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT+circle_root_key].setBrightness(1.0f);
				circleChanged=true;
			}

			
			if ((fvalue=std::round(params[CONTROL_SCALE_PARAM].getValue()))!=mode)
			{
				mode = fvalue;
				if (doDebug) DEBUG("mode changed to %d", mode);
				circleChanged=true;
			}


			

			// check input ports for change

			for (int i=0; i<Meander::NUM_INPUTS; ++i)
			{
				if (inputs[i].isConnected())
				{
					float fvalue=inputs[i].getVoltage();
								
					if ((i==IN_MELODY_SCALE_DEGREE_EXT_CV)||(fvalue!=inportStates[i].lastValue))  // don't do anything unless input changed
					{
						if (i!=IN_MELODY_SCALE_DEGREE_EXT_CV)
							inportStates[i].lastValue=fvalue;
						switch (i)
						{
							// process misc input ports
										
							case IN_TIMESIGNATURETOP_EXT_CV:
								if (fvalue>=0.01)
									{
										float ratio=(fvalue/10.0);
										float range=(13);
										int newValue=2 + (int)(ratio*range);
										newValue=clamp(newValue, 2, 15);
										if (newValue!=time_sig_top)
										{
											time_sig_top=newValue;  
											params[CONTROL_TIMESIGNATURETOP_PARAM].setValue((float)newValue);
											time_sig_changed=true;
										}
									}
									break;
							
							case IN_TIMESIGNATUREBOTTOM_EXT_CV:
								if (fvalue>=0.01)
									{
										float ratio=(fvalue/10.0);
										int exp=(int)(ratio*3);
										exp=clamp(exp, 0, 3);
										int newValue=pow(2,exp+1);

										if (newValue!=time_sig_bottom)
										{
											time_sig_bottom=newValue;  
											params[CONTROL_TIMESIGNATUREBOTTOM_PARAM].setValue((float)exp);
											int melody_note_length_divisor=0;
											if (time_sig_bottom==2)
											melody_note_length_divisor=1;
											else
											if (time_sig_bottom==4)
											melody_note_length_divisor=2;
											else
											if (time_sig_bottom==8)
											melody_note_length_divisor=3;
											else
											if (time_sig_bottom==16)
											melody_note_length_divisor=4; 
											theMeanderState.theMelodyParms.note_length_divisor=(int)std::pow(2,melody_note_length_divisor);
											params[CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM].setValue(melody_note_length_divisor);
											theMeanderState.theArpParms.note_length_divisor=(int)std::pow(2,melody_note_length_divisor+1);
											params[CONTROL_ARP_INCREMENT_PARAM].setValue(melody_note_length_divisor+1);
											time_sig_changed=true;
										}
									}
									break;


							// process harmony input ports

							
							

							case IN_HARMONY_ENABLE_EXT_CV:
								if (fvalue>0)
									theMeanderState.theHarmonyParms.enabled = true;
								else
								if (fvalue==0)
									theMeanderState.theHarmonyParms.enabled = false;
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_HARMONY_VOLUME_EXT_CV:
								if (fvalue>=0.01)
								if (fvalue!=theMeanderState.theHarmonyParms.volume)
								{
									fvalue=clamp(fvalue, 0., 10.);
									theMeanderState.theHarmonyParms.volume=fvalue;  
									params[CONTROL_HARMONY_VOLUME_PARAM].setValue(fvalue);
									outputs[OUT_HARMONY_VOLUME_OUTPUT].setVoltage(theMeanderState.theHarmonyParms.volume);
								}
								break;

							case IN_HARMONY_STEPS_EXT_CV:
								if (fvalue>0)
								{
									float ratio=(fvalue/10.0);
									float range=(theActiveHarmonyType.max_steps-theActiveHarmonyType.min_steps);
									int newValue=theActiveHarmonyType.min_steps + (int)(ratio*range);
									newValue=clamp(newValue, theActiveHarmonyType.min_steps, theActiveHarmonyType.max_steps);
									if (newValue!=params[CONTROL_HARMONY_STEPS_PARAM].getValue())
									{
										if ((newValue>=theActiveHarmonyType.min_steps)&&(newValue<=theActiveHarmonyType.max_steps))
										{
											theActiveHarmonyType.num_harmony_steps=(int)newValue;
											params[CONTROL_HARMONY_STEPS_PARAM].setValue(newValue);
										}
									}
								}
								else
								{
									// Do nothing.  Allow local control
								}
								break;
							
							case IN_HARMONY_TARGETOCTAVE_EXT_CV:
								if (fvalue>=0.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+ (int)(ratio*5);
									newValue=clamp(newValue, 1, 6);
									if (newValue!=theMeanderState.theHarmonyParms.target_octave)
									{
										theMeanderState.theHarmonyParms.target_octave=(int)newValue;  
										theMeanderState.theHarmonyParms.note_avg_target=theMeanderState.theHarmonyParms.target_octave/10.0;
										theMeanderState.theHarmonyParms.range_top=    theMeanderState.theHarmonyParms.note_avg_target + (theMeanderState.theHarmonyParms.note_octave_range/10.0);
										theMeanderState.theHarmonyParms.range_bottom= theMeanderState.theHarmonyParms.note_avg_target - (theMeanderState.theHarmonyParms.note_octave_range/10.0);
										theMeanderState.theHarmonyParms.r1=(theMeanderState.theHarmonyParms.range_top-theMeanderState.theHarmonyParms.range_bottom); 
										params[CONTROL_HARMONY_TARGETOCTAVE_PARAM].setValue(newValue);
									}
								}
								break;

							case IN_HARMONY_ALPHA_EXT_CV:
								if (fvalue>=0.01)
								{
									float newValue=(fvalue/10.0);
									newValue=clamp(newValue, 0., 1.);
									if (newValue!=theMeanderState.theHarmonyParms.alpha)
									{
										theMeanderState.theHarmonyParms.alpha=newValue;  
										params[CONTROL_HARMONY_ALPHA_PARAM].setValue(newValue);
									}
								}
								break;
							
							case IN_HARMONY_RANGE_EXT_CV:
								if (fvalue>=0.01)
								{
									float ratio=(fvalue/10.0);
									float newValue=ratio*3;
									newValue=clamp(newValue, 0., 3.);
									if (newValue!=theMeanderState.theHarmonyParms.note_octave_range)
									{
										theMeanderState.theHarmonyParms.note_octave_range=newValue;  

										theMeanderState.theHarmonyParms.note_avg_target=theMeanderState.theHarmonyParms.target_octave/10.0;
										theMeanderState.theHarmonyParms.range_top=    theMeanderState.theHarmonyParms.note_avg_target + (theMeanderState.theHarmonyParms.note_octave_range/10.0);
										theMeanderState.theHarmonyParms.range_bottom= theMeanderState.theHarmonyParms.note_avg_target - (theMeanderState.theHarmonyParms.note_octave_range/10.0);
										theMeanderState.theHarmonyParms.r1=(theMeanderState.theHarmonyParms.range_top-theMeanderState.theHarmonyParms.range_bottom); 

										params[CONTROL_HARMONY_RANGE_PARAM].setValue(newValue);
									}
								}
								break;

							case IN_HARMONY_DIVISOR_EXT_CV:
								if (fvalue>=0.01)
								{
								//	float ratio=(fvalue/10.0);
									float ratio=(fvalue/9.0); // allow for CV that doesn't quite get to 10.0
									int exp=(int)(ratio*3);
									exp=clamp(exp, 0, 3);
								//	int exp=(int)(ratio*4);
								//	exp=clamp(exp, 0, 4);
									int newValue=pow(2,exp);

									if (newValue!=theMeanderState.theHarmonyParms.note_length_divisor)
									{
										theMeanderState.theHarmonyParms.note_length_divisor=newValue;  
										params[CONTROL_HARMONY_DIVISOR_PARAM].setValue((float)exp);
									}
								}
								break;

							case IN_ENABLE_HARMONY_ALL7THS_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theHarmonyParms.enable_all_7ths = true;
									theMeanderState.theHarmonyParms.enable_V_7ths=false;
									setup_harmony();  // calculate harmony notes
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theHarmonyParms.enable_all_7ths = false;
									setup_harmony();  // calculate harmony notes
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_ENABLE_HARMONY_V7THS_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theHarmonyParms.enable_V_7ths = true;
									theMeanderState.theHarmonyParms.enable_all_7ths=false;
									setup_harmony();  // calculate harmony notes
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theHarmonyParms.enable_V_7ths = false;
									setup_harmony();  // calculate harmony notes
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_ENABLE_HARMONY_STACCATO_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theHarmonyParms.enable_staccato = true;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theHarmonyParms.enable_staccato = false;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_HARMONYPRESETS_EXT_CV:
								if (fvalue>=0.01)
								{
									float ratio=(fvalue/10.0);
									float newValue=1.+ (ratio*(MAX_AVAILABLE_HARMONY_PRESETS-1));
									newValue=clamp(newValue, 1., (float)MAX_AVAILABLE_HARMONY_PRESETS);
									if ((int)newValue!=harmony_type)
									{
										if (doDebug) DEBUG("getVoltage harmony type=%d", (int)newValue);
										harmonyPresetChanged=(int)newValue;  // don't changed until between sequences.  The new harmony_type is in harmonyPresetChanged
									}
									else
									{
										// Ao nothing.  Allow local control
									}
								}
								break;

							// process melody input ports

							case IN_MELODY_ENABLE_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theMelodyParms.enabled = true;
									theMeanderState.userControllingMelody=false;
								}
								else
								if (fvalue==0)
									theMeanderState.theMelodyParms.enabled = false;
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_MELODY_SCALE_DEGREE_EXT_CV:
													
								if (inputs[IN_MELODY_SCALE_DEGREE_EXT_CV].isConnected() && inputs[IN_MELODY_SCALE_GATE_EXT_CV].isConnected())
								{
									scaleDegree=inputs[IN_MELODY_SCALE_DEGREE_EXT_CV].getVoltage();
									gateValue=inputs[IN_MELODY_SCALE_GATE_EXT_CV].getVoltage();

									float octave=(float)((int)(scaleDegree));  // from the keyboard
									if (octave>3)
										octave=3;
									if (octave<-3)
										octave=-3;
									bool degreeChanged=false; // assume false unless determined true below
									bool skipStep=false;

								
									if ((gateValue==scaleDegree)&&(scaleDegree>=1)&&(scaleDegree<=7.7))  // MarkovSeq or other 1-7V degree  degree.octave 1.0-7.7V
									{
										if (inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].inTransition)
										{
											if (fvalue==inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue)
											{
												// was in transition but now is not
												inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].inTransition=false;
												inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue=fvalue;
												theMeanderState.theMelodyParms.lastMelodyDegreeIn=fvalue;
												octave = (int)(10.0*std::fmod(scaleDegree, 1.0f));
												if (octave>7)
													octave=7;
												scaleDegree=(float)((int)scaleDegree);
												degreeChanged=true;  // not really, but replay the note below
											}
											else
											if (fvalue!=inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue)
											{
												inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue=fvalue;
											}
										}
										else
										{
											if (fvalue!=inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue)
											{
												inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].inTransition=true;
												inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue=fvalue;
											}
										}
									}
									else
									if ((gateValue==scaleDegree)&&((scaleDegree<1.0)||(scaleDegree>=8.0)))  // MarkovSeq or other 1-7V degree  degree.octave 1.0-7.7V  <1 or >=8V means skip step
									{
										inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue=fvalue;
										degreeChanged=true;
										skipStep=true;
									}
									else  // keyboard  C-B
									if (!(gateValue==scaleDegree))
									{
										float fgvalue=inputs[IN_MELODY_SCALE_GATE_EXT_CV].getVoltage();
										if (inportStates[IN_MELODY_SCALE_GATE_EXT_CV].inTransition)
										{
											if (fgvalue==inportStates[IN_MELODY_SCALE_GATE_EXT_CV].lastValue)
											{
												// was in transition but now is not
												inportStates[IN_MELODY_SCALE_GATE_EXT_CV].inTransition=false;
												inportStates[IN_MELODY_SCALE_GATE_EXT_CV].lastValue=fgvalue;
												if (fgvalue)  // gate has gone high
												{
													if ( scaleDegree==inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue) // the gate has changed but the degree has not
														degreeChanged=true;   // not really, but play like it has so it will be replayed below
												}
											}
											else
											if (fgvalue!=inportStates[IN_MELODY_SCALE_GATE_EXT_CV].lastValue)
											{
												inportStates[IN_MELODY_SCALE_GATE_EXT_CV].lastValue=fgvalue;
											}
										}
										else
										{
											if (fgvalue!=inportStates[IN_MELODY_SCALE_GATE_EXT_CV].lastValue)
											{
												inportStates[IN_MELODY_SCALE_GATE_EXT_CV].inTransition=true;
												inportStates[IN_MELODY_SCALE_GATE_EXT_CV].lastValue=fgvalue;
											}
										}

										if ( (degreeChanged) || (scaleDegree!=inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue))
										{
											inportStates[IN_MELODY_SCALE_DEGREE_EXT_CV].lastValue= scaleDegree;
											octave=(float)((int)(scaleDegree));  // from the keyboard
											if (octave>3)
												octave=3;
											if (octave<-3)
												octave=-3;
											if (scaleDegree>=0)
												scaleDegree=(float)std::fmod(std::fabs(scaleDegree), 1.0f);
											else
												scaleDegree=-(float)std::fmod(std::fabs(scaleDegree), 1.0f);
											degreeChanged=true; 
											if (doDebug) DEBUG("IN_MELODY_SCALE_DEGREE_EXT_CV scaleDegree=%f", scaleDegree);
											if (scaleDegree>=0)
											{
												if ((std::abs(scaleDegree)<.005f))   scaleDegree=1;
												else
												if ((std::abs(scaleDegree-.167f)<.005f))  scaleDegree=2;
												else
												if ((std::abs(scaleDegree-.333f)<.005f))  scaleDegree=3;
												else
												if ((std::abs(scaleDegree-.417f)<.005f))  scaleDegree=4;
												else
												if ((std::abs(scaleDegree-.583f)<.005f))  scaleDegree=5;
												else
												if ((std::abs(scaleDegree-.750f)<.005f))  scaleDegree=6;
												else
												if ((std::abs(scaleDegree-.917f)<.005f))  scaleDegree=7;
												else
													degreeChanged=false;
											}
											else
											{
												octave-=1;
												if ((std::abs(scaleDegree)<.005f))   scaleDegree=1;
												else
											    if (std::abs(std::abs(scaleDegree)-.083)<.005f)  scaleDegree=7;
												else
												if (std::abs(std::abs(scaleDegree)-.250)<.005f)  scaleDegree=6;
												else
											    if (std::abs(std::abs(scaleDegree)-.417)<.005f)  scaleDegree=5;
												else
											    if (std::abs(std::abs(scaleDegree)-.583)<.005f)  scaleDegree=4;
												else
											    if (std::abs(std::abs(scaleDegree)-.667)<.005f)  scaleDegree=3;
												else
											    if (std::abs(std::abs(scaleDegree)-.833)<.005f)  scaleDegree=2;
												else
													degreeChanged=false;
											}
											
										}	
									}

																
									if ((degreeChanged)&&(!skipStep))  
									{
										if (scaleDegree<1)
											scaleDegree=1;
										if (scaleDegree>7)
											scaleDegree=7;

										if (doDebug) DEBUG("IN_HARMONY_CIRCLE_DEGREE_EXT_CV=%d", (int)theMeanderState.circleDegree);
									    //	DEBUG("IN_HARMONY_CIRCLE_DEGREE_EXT_CV=%d", (int)theMeanderState.circleDegree);
																	
										if (scaleDegree>0)
										{
											userPlaysScaleDegreeMelody(scaleDegree, octave+theMeanderState.theMelodyParms.target_octave); 
											theMeanderState.theArpParms.note_count=0; 
										}
										
										if (running)
										{
											if (theMeanderState.theMelodyParms.enabled)
												theMeanderState.theMelodyParms.enabled = false;
											theMeanderState.userControllingMelody=true;
										}
										
									}
								}

								

								break;

							case IN_MELODY_VOLUME_EXT_CV:
								if (fvalue>=.01)
								if (fvalue!=theMeanderState.theMelodyParms.volume)
								{
									fvalue=clamp(fvalue, 0., 10.);
									theMeanderState.theMelodyParms.volume=fvalue;  
									params[CONTROL_MELODY_VOLUME_PARAM].setValue(fvalue);
									outputs[OUT_MELODY_VOLUME_OUTPUT].setVoltage(theMeanderState.theMelodyParms.volume);
								}
								break;

							case IN_MELODY_NOTE_LENGTH_DIVISOR_EXT_CV:
								if (fvalue>=.01)
								{
								//	float ratio=(fvalue/10.0);
									float ratio=(fvalue/9.0);  // allow for CV that doesn't quite get to 10V
									int exp=(int)(ratio*5);
									exp=clamp(exp, 0, 5);
									int newValue=pow(2,exp);

									if (newValue!=theMeanderState.theMelodyParms.note_length_divisor)
									{
										theMeanderState.theMelodyParms.note_length_divisor=newValue;  
										params[CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM].setValue((float)exp);
									}
								}
								break;

							case IN_MELODY_TARGETOCTAVE_EXT_CV:
								if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+ (int)(ratio*5);
									newValue=clamp(newValue, 1, 6);
									if (newValue!=theMeanderState.theMelodyParms.target_octave)
									{
										theMeanderState.theMelodyParms.target_octave=(int)newValue;  
										theMeanderState.theMelodyParms.note_avg_target=theMeanderState.theMelodyParms.target_octave/10.0;
										theMeanderState.theMelodyParms.range_top=    theMeanderState.theMelodyParms.note_avg_target + (theMeanderState.theMelodyParms.note_octave_range/10.0);
										theMeanderState.theMelodyParms.range_bottom= theMeanderState.theMelodyParms.note_avg_target - (theMeanderState.theMelodyParms.note_octave_range/10.0);
										theMeanderState.theMelodyParms.r1=(theMeanderState.theMelodyParms.range_top-theMeanderState.theMelodyParms.range_bottom); 
										params[CONTROL_MELODY_TARGETOCTAVE_PARAM].setValue(newValue);
									}
								}
								break;

							case IN_MELODY_ALPHA_EXT_CV:
								if (fvalue>=.01)
								{
									float newValue=(fvalue/10.0);
									newValue=clamp(newValue, 0., 1.);
									if (newValue!=theMeanderState.theMelodyParms.alpha)
									{
										theMeanderState.theMelodyParms.alpha=newValue;  
										params[CONTROL_MELODY_ALPHA_PARAM].setValue(newValue);
									}
								}
								break;
							
							case IN_MELODY_RANGE_EXT_CV:
								if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									float newValue=ratio*3;
									newValue=clamp(newValue, 0., 3.);
									if (newValue!=theMeanderState.theMelodyParms.note_octave_range)
									{
										theMeanderState.theMelodyParms.note_octave_range=newValue;  

										theMeanderState.theMelodyParms.note_avg_target=theMeanderState.theMelodyParms.target_octave/10.0;
										theMeanderState.theMelodyParms.range_top=    theMeanderState.theMelodyParms.note_avg_target + (theMeanderState.theMelodyParms.note_octave_range/10.0);
										theMeanderState.theMelodyParms.range_bottom= theMeanderState.theMelodyParms.note_avg_target - (theMeanderState.theMelodyParms.note_octave_range/10.0);
										theMeanderState.theMelodyParms.r1=(theMeanderState.theMelodyParms.range_top-theMeanderState.theMelodyParms.range_bottom); 

										params[CONTROL_MELODY_RANGE_PARAM].setValue(newValue);
									}
								}
								break;

							////

							case IN_MELODY_DESTUTTER_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theMelodyParms.destutter = true;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theMelodyParms.destutter = false;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;
							

							case IN_ENABLE_MELODY_STACCATO_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theMelodyParms.enable_staccato = true;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theMelodyParms.enable_staccato = false;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;
							
							case IN_ENABLE_MELODY_CHORDAL_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theMelodyParms.chordal = true;
									theMeanderState.theMelodyParms.scaler = false;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theMelodyParms.chordal = false;
									theMeanderState.theMelodyParms.scaler = true;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_MELODY_SCALER_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theMelodyParms.scaler = true;
									theMeanderState.theMelodyParms.chordal = false;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theMelodyParms.scaler = false;
									theMeanderState.theMelodyParms.chordal = true;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;
							

							// process arp input ports

							case IN_ARP_ENABLE_EXT_CV:
								if (fvalue>0)
									theMeanderState.theArpParms.enabled = true;
								else
								if (fvalue==0)
									theMeanderState.theArpParms.enabled = false;
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_ARP_COUNT_EXT_CV:
								if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
								//	int newValue=(int)(ratio*7);
									int newValue=(int)(ratio*31);
								//	newValue=clamp(newValue, 0, 7);
									newValue=clamp(newValue, 0, 31);
									
									if (newValue!=theMeanderState.theArpParms.count)
									{
										theMeanderState.theArpParms.count=newValue;  
										params[CONTROL_ARP_COUNT_PARAM].setValue((float)newValue);
									}
								}
								break;


							case IN_ARP_INCREMENT_EXT_CV:
								if (fvalue>=.01)
								{
								//	float ratio=(fvalue/10.0);
									float ratio=(fvalue/9.0);  // allow for CV that doesn't quite get to 10.0
									int exp=(int)(ratio*3);
									exp=clamp(exp, 0, 3)+2;
									int newValue=pow(2,exp);

									if (newValue!=theMeanderState.theArpParms.note_length_divisor)
									{
										theMeanderState.theArpParms.note_length_divisor=newValue;  
										params[CONTROL_ARP_INCREMENT_PARAM].setValue((float)exp);
									}
								}
								break;

							case IN_ARP_DECAY_EXT_CV:
								if (fvalue>=.01)
								{
									float newValue=(fvalue/10.0);
									newValue=clamp(newValue, 0., 1.);
									if (newValue!=theMeanderState.theArpParms.decay)
									{
										theMeanderState.theArpParms.decay=newValue;  
										params[CONTROL_ARP_DECAY_PARAM].setValue(newValue);
									}
								}
								break;

							case IN_ENABLE_ARP_CHORDAL_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theArpParms.chordal = true;
									theMeanderState.theArpParms.scaler = false;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theArpParms.chordal = false;
									theMeanderState.theArpParms.scaler = true;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_ENABLE_ARP_SCALER_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theArpParms.scaler = true;
									theMeanderState.theArpParms.chordal = false;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theArpParms.scaler = false;
									theMeanderState.theArpParms.chordal = true;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_ARP_PATTERN_EXT_CV:  // issue with range of -3 to +3
							    if (true)  // just for local variable scope
								{
								//	float ratio=(fvalue/10.0);
									float ratio=(fvalue/9.0);  // handle CV's that do not quite make it to 10.0V
								//	int newValue=(int)(ratio*3);
								//	newValue=clamp(newValue, -3, 3);
								//	int newValue=(int)(ratio*2);
									int newValue=(int)(ratio*4);
									newValue -= 2;
									newValue=clamp(newValue, -2, 2);
									if (newValue!=theMeanderState.theArpParms.pattern)
									{
										theMeanderState.theArpParms.pattern=newValue;  
										params[CONTROL_ARP_PATTERN_PARAM].setValue(newValue);
									}
								}

							    break;

							
							// process bass input ports

							case IN_BASS_ENABLE_EXT_CV:
								if (fvalue>0)
									theMeanderState.theBassParms.enabled = true;
								else
								if (fvalue==0)
									theMeanderState.theBassParms.enabled = false;
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_BASS_VOLUME_EXT_CV:
								if (fvalue>=.01)
								if (fvalue!=theMeanderState.theBassParms.volume)
								{
									fvalue=clamp(fvalue, 0., 10.);
									theMeanderState.theBassParms.volume=fvalue;  
									params[CONTROL_BASS_VOLUME_PARAM].setValue(fvalue);
									outputs[OUT_BASS_VOLUME_OUTPUT].setVoltage(theMeanderState.theBassParms.volume);
								}
								break;

							case IN_BASS_TARGETOCTAVE_EXT_CV:
								if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+ (int)(ratio*5);
									newValue=clamp(newValue, 1, 6);
									if (newValue!=theMeanderState.theBassParms.target_octave)
									{
										theMeanderState.theBassParms.target_octave=(int)newValue;  
										params[CONTROL_BASS_TARGETOCTAVE_PARAM].setValue(newValue);
									}
								}
								break;

							case IN_BASS_DIVISOR_EXT_CV:
								if (fvalue>=.01)
								{
									//float ratio=(fvalue/10.0);
									float ratio=(fvalue/9.0); // allow for CV that doesn't quite get to 10.0
									int exp=(int)(ratio*3);
									exp=clamp(exp, 0, 3);
									int newValue=pow(2,exp);

									if (newValue!=theMeanderState.theBassParms.note_length_divisor)
									{
										theMeanderState.theBassParms.note_length_divisor=newValue;  
										params[CONTROL_BASS_DIVISOR_PARAM].setValue((float)exp);
									}
								}
								break;
							
							case IN_ENABLE_BASS_STACCATO_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theBassParms.enable_staccato = true;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theBassParms.enable_staccato = false;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_BASS_ACCENT_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theBassParms.accent = true;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theBassParms.accent = false;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;


							case IN_BASS_SYNCOPATE_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theBassParms.syncopate = true;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theBassParms.syncopate = false;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_BASS_SHUFFLE_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theBassParms.shuffle = true;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theBassParms.shuffle = false;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							case IN_BASS_OCTAVES_EXT_CV:
								if (fvalue>0)
								{
									theMeanderState.theBassParms.octave_enabled = true;
								}
								else
								if (fvalue==0)
								{
									theMeanderState.theBassParms.octave_enabled = false;
								}
								else
								if (fvalue<0) 
								{
									// Do nothing.  Allow local parameter control
								}
								break;

							// process fBn input ports	

							case IN_HARMONY_FBM_OCTAVES_EXT_CV:
							if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+(int)(ratio*5);
									newValue=clamp(newValue, 1, 6);
									
									if (newValue!=theMeanderState.theHarmonyParms.noctaves)
									{
										theMeanderState.theHarmonyParms.noctaves=newValue;  
										params[CONTROL_HARMONY_FBM_OCTAVES_PARAM].setValue((float)newValue);
									}
								}
								break;

							case IN_HARMONY_FBM_PERIOD_EXT_CV:
							if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+(int)(ratio*100);
									newValue=clamp(newValue, 1, 100);
									
									if (newValue!=theMeanderState.theHarmonyParms.period)
									{
										theMeanderState.theHarmonyParms.period=newValue;  
										params[CONTROL_HARMONY_FBM_PERIOD_PARAM].setValue((float)newValue);
									}
								}
								break;

							case IN_MELODY_FBM_OCTAVES_EXT_CV:
							if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+(int)(ratio*5);
									newValue=clamp(newValue, 1, 6);
									
									if (newValue!=theMeanderState.theMelodyParms.noctaves)
									{
										theMeanderState.theMelodyParms.noctaves=newValue;  
										params[CONTROL_MELODY_FBM_OCTAVES_PARAM].setValue((float)newValue);
									}
								}
								break;

							case IN_MELODY_FBM_PERIOD_EXT_CV:
							if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+(int)(ratio*100);
									newValue=clamp(newValue, 1, 100);
									
									if (newValue!=theMeanderState.theMelodyParms.period)
									{
										theMeanderState.theMelodyParms.period=newValue;  
										params[CONTROL_MELODY_FBM_PERIOD_PARAM].setValue((float)newValue);
									}
								}
								break;

							case IN_ARP_FBM_OCTAVES_EXT_CV:
							if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+(int)(ratio*5);
									newValue=clamp(newValue, 1, 6);
									
									if (newValue!=theMeanderState.theArpParms.noctaves)
									{
										theMeanderState.theArpParms.noctaves=newValue;  
										params[CONTROL_ARP_FBM_OCTAVES_PARAM].setValue((float)newValue);
									}
								}
								break;

							case IN_ARP_FBM_PERIOD_EXT_CV:
							if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=1+(int)(ratio*100);
									newValue=clamp(newValue, 1, 100);
									
									if (newValue!=theMeanderState.theArpParms.period)
									{
										theMeanderState.theArpParms.period=newValue;  
										params[CONTROL_ARP_FBM_PERIOD_PARAM].setValue((float)newValue);
									}
								}
								break;

						    // handle mode and root input changes
						  
						    case IN_ROOT_KEY_EXT_CV:
						    if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=(int)(ratio*11);
									newValue=clamp(newValue, 0, 11);
									if (newValue!=circle_root_key)
									{
										circle_root_key=(int)newValue;
										root_key=circle_of_fifths[circle_root_key];
										params[CONTROL_ROOT_KEY_PARAM].setValue(circle_root_key);
										if (doDebug) DEBUG("root_key changed to %d = %s", root_key, note_desig[root_key]);
										for (int i=0; i<12; ++i)
											lights[LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT+i].setBrightness(0.0f);
										lights[LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT+circle_root_key].setBrightness(1.0f);
										circleChanged=true;
									}
								}
					        	break;

							case IN_SCALE_EXT_CV:
						    if (fvalue>=.01)
								{
									float ratio=(fvalue/10.0);
									int newValue=(int)(ratio*6);
									newValue=clamp(newValue, 0, 6);
									if (newValue!=mode)
									{
										mode=(int)newValue;
										params[CONTROL_SCALE_PARAM].setValue(mode);
										circleChanged=true;
									}
								}
					        	break;

							


						};  // end switch
					}
				}
			}
		
			// harmony params

			fvalue=(params[CONTROL_HARMONY_VOLUME_PARAM].getValue());
			if (fvalue!=theMeanderState.theHarmonyParms.volume)
			{
				theMeanderState.theHarmonyParms.volume=fvalue;  
				outputs[OUT_HARMONY_VOLUME_OUTPUT].setVoltage(theMeanderState.theHarmonyParms.volume);
			}

			fvalue=std::round(params[CONTROL_HARMONY_TARGETOCTAVE_PARAM].getValue());
			if (fvalue!=theMeanderState.theHarmonyParms.target_octave)
			{
				theMeanderState.theHarmonyParms.target_octave=fvalue;  
				theMeanderState.theHarmonyParms.note_avg_target=theMeanderState.theHarmonyParms.target_octave/10.0;
				theMeanderState.theHarmonyParms.range_top=    theMeanderState.theHarmonyParms.note_avg_target + (theMeanderState.theHarmonyParms.note_octave_range/10.0);
				theMeanderState.theHarmonyParms.range_bottom= theMeanderState.theHarmonyParms.note_avg_target - (theMeanderState.theHarmonyParms.note_octave_range/10.0);
				theMeanderState.theHarmonyParms.r1=(theMeanderState.theHarmonyParms.range_top-theMeanderState.theHarmonyParms.range_bottom); 
			}

			
			fvalue=params[CONTROL_HARMONY_DIVISOR_PARAM].getValue();
			ivalue=(int)fvalue;
			ivalue=pow(2,ivalue);
			if ((ivalue)!=theMeanderState.theHarmonyParms.note_length_divisor)
			{
				theMeanderState.theHarmonyParms.note_length_divisor=ivalue;  
			}

			fvalue=(params[CONTROL_HARMONY_RANGE_PARAM].getValue());
			if (fvalue!=theMeanderState.theMelodyParms.note_octave_range)
			{
				theMeanderState.theHarmonyParms.note_octave_range=fvalue;  
				theMeanderState.theHarmonyParms.note_avg_target=theMeanderState.theHarmonyParms.target_octave/10.0;
				theMeanderState.theHarmonyParms.range_top=    theMeanderState.theHarmonyParms.note_avg_target + (theMeanderState.theHarmonyParms.note_octave_range/10.0);
				theMeanderState.theHarmonyParms.range_bottom= theMeanderState.theHarmonyParms.note_avg_target - (theMeanderState.theHarmonyParms.note_octave_range/10.0);
				theMeanderState.theHarmonyParms.r1=(theMeanderState.theHarmonyParms.range_top-theMeanderState.theHarmonyParms.range_bottom); 
			}

			fvalue=(params[CONTROL_HARMONY_ALPHA_PARAM].getValue());
			if ((fvalue)!=theMeanderState.theHarmonyParms.alpha)
			{
				theMeanderState.theHarmonyParms.alpha=fvalue;  
			}

			fvalue=params[CONTROL_HARMONY_FBM_OCTAVES_PARAM].getValue();
			if (fvalue!=theMeanderState.theHarmonyParms.noctaves)
			{
				theMeanderState.theHarmonyParms.noctaves=fvalue;  
			}

			fvalue=params[CONTROL_HARMONY_FBM_PERIOD_PARAM].getValue();
			if (fvalue!=theMeanderState.theHarmonyParms.period)
			{
				theMeanderState.theHarmonyParms.period=fvalue;  
			}
			//********* 

			if ((fvalue=std::round(params[CONTROL_HARMONYPRESETS_PARAM].getValue()))!=harmony_type)
			{
				if (doDebug) DEBUG(" getValue harmony type=%d", (int)fvalue);
			   	harmonyPresetChanged=(int)fvalue;  // don't changed until between sequences.  The new harmony_type is in harmonyPresetChanged
			}

			fvalue=std::round(params[CONTROL_HARMONY_STEPS_PARAM].getValue());
			if ((fvalue!=theActiveHarmonyType.num_harmony_steps)&&(fvalue>=theActiveHarmonyType.min_steps)&&(fvalue<=theActiveHarmonyType.max_steps)&&(fvalue!=theActiveHarmonyType.num_harmony_steps))
			{
				if (doDebug) DEBUG("theActiveHarmonyType.min_steps=%d, theActiveHarmonyType.max_steps=%d", theActiveHarmonyType.min_steps, theActiveHarmonyType.max_steps );
				if (doDebug) DEBUG("theActiveHarmonyType.num_harmony_steps changed to %d %s", (int)fvalue, "test");  // need actual descriptor
				if ((fvalue>=theActiveHarmonyType.min_steps)&&(fvalue<=theActiveHarmonyType.max_steps))
					theActiveHarmonyType.num_harmony_steps=(int)fvalue;  
			}

			// Melody Params

			fvalue=(params[CONTROL_MELODY_VOLUME_PARAM].getValue());
			if (fvalue!=theMeanderState.theMelodyParms.volume)
			{
				theMeanderState.theMelodyParms.volume=fvalue;  
				outputs[OUT_MELODY_VOLUME_OUTPUT].setVoltage(theMeanderState.theMelodyParms.volume);
			}
			
			fvalue=std::round(params[CONTROL_MELODY_TARGETOCTAVE_PARAM].getValue());
			if (fvalue!=theMeanderState.theMelodyParms.target_octave)
			{
				theMeanderState.theMelodyParms.target_octave=fvalue;  
				theMeanderState.theMelodyParms.note_avg_target=theMeanderState.theMelodyParms.target_octave/10.0;
				theMeanderState.theMelodyParms.range_top=    theMeanderState.theMelodyParms.note_avg_target + (theMeanderState.theMelodyParms.note_octave_range/10.0);
				theMeanderState.theMelodyParms.range_bottom= theMeanderState.theMelodyParms.note_avg_target - (theMeanderState.theMelodyParms.note_octave_range/10.0);
				theMeanderState.theMelodyParms.r1=(theMeanderState.theMelodyParms.range_top-theMeanderState.theMelodyParms.range_bottom); 
			}

			fvalue=(params[CONTROL_MELODY_RANGE_PARAM].getValue());
			if (fvalue!=theMeanderState.theMelodyParms.note_octave_range)
			{
				theMeanderState.theMelodyParms.note_octave_range=fvalue;  
				theMeanderState.theMelodyParms.note_avg_target=theMeanderState.theMelodyParms.target_octave/10.0;
				theMeanderState.theMelodyParms.range_top=    theMeanderState.theMelodyParms.note_avg_target + (theMeanderState.theMelodyParms.note_octave_range/10.0);
				theMeanderState.theMelodyParms.range_bottom= theMeanderState.theMelodyParms.note_avg_target - (theMeanderState.theMelodyParms.note_octave_range/10.0);
				theMeanderState.theMelodyParms.r1=(theMeanderState.theMelodyParms.range_top-theMeanderState.theMelodyParms.range_bottom); 
			}

			fvalue=(params[CONTROL_MELODY_ALPHA_PARAM].getValue());
			if ((fvalue)!=theMeanderState.theMelodyParms.alpha)
			{
				theMeanderState.theMelodyParms.alpha=fvalue;  
			}
			
			fvalue=params[CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM].getValue();
			ivalue=(int)fvalue;
			ivalue=pow(2,ivalue);
			if ((ivalue)!=theMeanderState.theMelodyParms.note_length_divisor)
			{
				theMeanderState.theMelodyParms.note_length_divisor=ivalue;  
			}

			fvalue=params[CONTROL_MELODY_FBM_OCTAVES_PARAM].getValue();
			if (fvalue!=theMeanderState.theMelodyParms.noctaves)
			{
				theMeanderState.theMelodyParms.noctaves=fvalue;  
			}

			fvalue=params[CONTROL_MELODY_FBM_PERIOD_PARAM].getValue();
			if (fvalue!=theMeanderState.theMelodyParms.period)
			{
				theMeanderState.theMelodyParms.period=fvalue;  
			}

           														
			// bass params***********

			fvalue=(params[CONTROL_BASS_VOLUME_PARAM].getValue());
			if (fvalue!=theMeanderState.theBassParms.volume)
			{
				theMeanderState.theBassParms.volume=fvalue;  
				outputs[OUT_BASS_VOLUME_OUTPUT].setVoltage(theMeanderState.theBassParms.volume);
			}

			fvalue=params[CONTROL_BASS_DIVISOR_PARAM].getValue();
			ivalue=(int)fvalue;
			ivalue=pow(2,ivalue);
			if ((ivalue)!=theMeanderState.theBassParms.note_length_divisor)
			{
				theMeanderState.theBassParms.note_length_divisor=ivalue;  
			}


			fvalue=std::round(params[CONTROL_BASS_TARGETOCTAVE_PARAM].getValue());
			if (fvalue!=theMeanderState.theBassParms.target_octave)
			{
				theMeanderState.theBassParms.target_octave=fvalue;  
			}

			
			fvalue=std::round(params[CONTROL_ARP_COUNT_PARAM].getValue());
			if (fvalue!=theMeanderState.theArpParms.count)
			{
				theMeanderState.theArpParms.count=fvalue;  
			}

			fvalue=std::round(params[CONTROL_ARP_INCREMENT_PARAM].getValue());
			ivalue=(int)fvalue;
			ivalue=pow(2,ivalue);
			if (ivalue!=theMeanderState.theArpParms.note_length_divisor)
			{
				theMeanderState.theArpParms.note_length_divisor=ivalue;  
			}

			fvalue=(params[	CONTROL_ARP_DECAY_PARAM].getValue());
			if (fvalue!=theMeanderState.theArpParms.decay)
			{
				theMeanderState.theArpParms.decay=fvalue;  
			}

			fvalue=std::round(params[CONTROL_ARP_PATTERN_PARAM].getValue());
			if (fvalue!=theMeanderState.theArpParms.pattern)
			{
				theMeanderState.theArpParms.pattern=fvalue;  
			}

			fvalue=params[CONTROL_ARP_FBM_OCTAVES_PARAM].getValue();
			if (fvalue!=theMeanderState.theArpParms.noctaves)
			{
				theMeanderState.theArpParms.noctaves=fvalue;  
			}

			fvalue=params[CONTROL_ARP_FBM_PERIOD_PARAM].getValue();
			if (fvalue!=theMeanderState.theArpParms.period)
			{
				theMeanderState.theArpParms.period=fvalue;  
			}

			// **************************

			if (harmonyPresetChanged)
			{
				harmony_type=harmonyPresetChanged;
				copyHarmonyTypeToActiveHarmonyType(harmony_type);
				harmonyPresetChanged=0;
				circleChanged=true;  // trigger off reconstruction and setup
				init_harmony(); // reinitialize in case user has changed harmony parms
				setup_harmony();  // calculate harmony notes
				params[CONTROL_HARMONYPRESETS_PARAM].setValue(harmony_type);
				params[CONTROL_HARMONY_STEPS_PARAM].setValue(theHarmonyTypes[harmony_type].num_harmony_steps);
				time_sig_changed=true;  // forces a reset so things start over
			//	AuditHarmonyData(2);
			}
			
			// reconstruct initially and when dirty
			if (circleChanged)  
			{	
				if (doDebug) DEBUG("circleChanged");	
				
				notate_mode_as_signature_root_key=((root_key-(mode_natural_roots[mode_root_key_signature_offset[mode]]))+12)%12;
				if (doDebug) DEBUG("notate_mode_as_signature_root_key=%d", notate_mode_as_signature_root_key);
				
				if ((notate_mode_as_signature_root_key==1)   // Db
				  ||(notate_mode_as_signature_root_key==3)   // Eb
				  ||(notate_mode_as_signature_root_key==5)   // F
				  ||(notate_mode_as_signature_root_key==8)   // Ab
				  ||(notate_mode_as_signature_root_key==10)) // Bb
				{
					for (int i=0; i<12; ++i)
						strcpy(note_desig[i], note_desig_flats[i]);
				}
				else
				{
					for (int i=0; i<12; ++i)
						strcpy(note_desig[i], note_desig_sharps[i]);
				}

				ConstructCircle5ths(circle_root_key, mode);
				ConstructDegreesSemicircle(circle_root_key, mode); //int circleroot_key, int mode)
				init_notes();  // depends on mode and root_key			
				init_harmony();  // sets up original progressions
				AuditHarmonyData(3);
				setup_harmony();  // calculate harmony notes
				if (savedHarmonySteps)
				{
					params[CONTROL_HARMONY_STEPS_PARAM].setValue(savedHarmonySteps);
					savedHarmonySteps = 0;
				}
				else
					params[CONTROL_HARMONY_STEPS_PARAM].setValue(theHarmonyTypes[harmony_type].num_harmony_steps);
				AuditHarmonyData(3);
				circleChanged=false;

			}

			// send Poly External Scale to output  // using Aria standard
			
			// outputs[OUT_EXT_POLY_SCALE_OUTPUT].setChannels(12);  // set polyphony
			
			// for (int i=0; i<12; ++i)
			// {
			// 	outputs[OUT_EXT_POLY_SCALE_OUTPUT].setVoltage(0.0,i);  // (not scale note, channel) 
			// }
			// for (int i=0;i<mode_step_intervals[mode][0];++i)
			// {
			// 	int note=(int)(notes[i]%MAX_NOTES);  
			// 	if (note==root_key)
			// 		outputs[OUT_EXT_POLY_SCALE_OUTPUT].setVoltage(10.0,(int)note);  // (scale note, channel) 
			// 	else
			// 		outputs[OUT_EXT_POLY_SCALE_OUTPUT].setVoltage(8.0,(int)note);  // (scale note, channel) 
			// }
			
		}	
		 	     
	}  // end module process()

	~Meander() 
	{
	}


	Meander() 
	{

		if (doDebug) DEBUG("");  // clear debug log file

	    instanceRunning = true;
		
		lowFreqClock.setDivision(512);  // every 86 samples, 2ms
				   		
		
		initPerlin();
		ConfigureGlobals();
		MeanderMusicStructuresInitialize();  // sets global globalsInitialized=true

			
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		for (int i=0; i<12; ++i)
			lights[LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT+i].setBrightness(0.0f);
		lights[LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT+root_key].setBrightness(1.0f);  // loaded root_key might not be 0/C
		
		CircleStepStates[0]=1.0f;
		lights[LIGHT_LEDBUTTON_CIRCLESTEP_1].setBrightness(1.0f);
		
		CircleStepSetStates[0]=1.0f;
		lights[LIGHT_LEDBUTTON_CIRCLESETSTEP_1].setBrightness(1.0f);

								
		configParam(BUTTON_RUN_PARAM, 0.f, 1.f, 0.f, "Run");
		configParam(BUTTON_RESET_PARAM, 0.f, 1.f, 0.f, "Reset");

		configParam(CONTROL_TEMPOBPM_PARAM, min_bpm, max_bpm, 120.0f, "Tempo", " BPM");
	    configParam(CONTROL_TIMESIGNATURETOP_PARAM,2.0f, 15.0f, 4.0f, "Time Signature Top");
		configParam(CONTROL_TIMESIGNATUREBOTTOM_PARAM,0.0f, 3.0f, 1.0f, "Time Signature Bottom");
	//	configParam(CONTROL_ROOT_KEY_PARAM, 0, 11, 1.f, "Root/Key");
		configParam(CONTROL_ROOT_KEY_PARAM, 0, 11, 0.f, "Root/Key");
		configParam(CONTROL_SCALE_PARAM, 0.f, num_modes-1, 1.f, "Mode");

		configParam(BUTTON_ENABLE_MELODY_PARAM, 0.f, 1.f, 0.f, "Enable");
		configParam(CONTROL_MELODY_VOLUME_PARAM, 0.f, 10.f, 8.0f, "Volume");
		configParam(BUTTON_MELODY_DESTUTTER_PARAM, 0.f, 1.f, 0.f, "Hold Tied Notes");
		configParam(CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM, 0.f, 5.f, 2.f, "Notes on 1/N");
		configParam(CONTROL_MELODY_TARGETOCTAVE_PARAM, 1.f, 6.f, 3.f, "Target Octave");
		configParam(CONTROL_MELODY_ALPHA_PARAM, 0.f, 1.f, .9f, "Variablility");
		configParam(CONTROL_MELODY_RANGE_PARAM, 0.f, 3.f, 1.f, "Octave Range");
		configParam(BUTTON_ENABLE_MELODY_STACCATO_PARAM, 0.f, 1.f, 1.f, "Staccato");

		configParam(BUTTON_ENABLE_HARMONY_PARAM, 0.f, 1.f, 0.f, "Enable");
		configParam(BUTTON_ENABLE_MELODY_CHORDAL_PARAM, 0.f, 1.f, 1.f, "Chordal Notes");
		configParam(BUTTON_ENABLE_MELODY_SCALER_PARAM, 0.f, 1.f, 0.f, "Scaler Notes");
		configParam(CONTROL_HARMONY_VOLUME_PARAM, 0.f, 10.f, 8.0f, "Volume (0-10)");
		configParam(CONTROL_HARMONY_STEPS_PARAM, 1.f, 16.f, 16.f, "Steps");
		configParam(CONTROL_HARMONY_TARGETOCTAVE_PARAM, 1.f, 6.f, 3.f, "Target Octave");
		configParam(CONTROL_HARMONY_ALPHA_PARAM, 0.f, 1.f, .9f, "Variability");
		configParam(CONTROL_HARMONY_RANGE_PARAM, 0.f, 3.f, 1.f, "Octave Range");
		configParam(CONTROL_HARMONY_DIVISOR_PARAM, 0.f, 3.f, 0.f, "Notes Length");
		configParam(BUTTON_ENABLE_HARMONY_ALL7THS_PARAM, 0.f, 1.f, 0.f, "7ths");
		configParam(BUTTON_ENABLE_HARMONY_V7THS_PARAM, 0.f, 1.f, 0.f, "V 7ths");
		configParam(BUTTON_ENABLE_HARMONY_STACCATO_PARAM, 0.f, 1.f, 0.f, "Staccato");
		configParam(CONTROL_HARMONYPRESETS_PARAM, 1.0f, (float)MAX_AVAILABLE_HARMONY_PRESETS, 1.0f, "Progression Preset");

		configParam(BUTTON_ENABLE_ARP_PARAM, 0.f, 1.f, 0.f, "Enable");
		configParam(BUTTON_ENABLE_ARP_CHORDAL_PARAM, 0.f, 1.f, 1.f, "Chordal Notes");
		configParam(BUTTON_ENABLE_ARP_SCALER_PARAM, 0.f, 1.f, 0.f, "Scaler Notes");
		configParam(CONTROL_ARP_COUNT_PARAM, 0.f, 31.f, 0.f, "Arp Notes Played");
		configParam(CONTROL_ARP_INCREMENT_PARAM, 2.f, 5.f, 4.f, "Arp Notes Length");
		configParam(CONTROL_ARP_DECAY_PARAM, 0.f, 1.f, 0.f, "Volume Decay");
		configParam(CONTROL_ARP_PATTERN_PARAM, -3.f, 3.f, 1.f, "Pattern Preset");

		configParam(BUTTON_ENABLE_BASS_PARAM, 0.f, 1.f, 0.f, "Enable");
		configParam(CONTROL_BASS_VOLUME_PARAM, 0.f, 10.f, 8.0f, "Volume");
		configParam(CONTROL_BASS_DIVISOR_PARAM, 0.f, 3.f, 0.f, "Notes Length");
		configParam(CONTROL_BASS_TARGETOCTAVE_PARAM, 0.f, 3.f, 2.f, "Target Octave"); 
		configParam(BUTTON_BASS_ACCENT_PARAM, 0.f, 1.f, 0.f, "Accent");
		configParam(BUTTON_BASS_SYNCOPATE_PARAM, 0.f, 1.f, 0.f, "Syncopate");
		configParam(BUTTON_BASS_SHUFFLE_PARAM, 0.f, 1.f, 0.f, "Shuffle");
		configParam(BUTTON_ENABLE_BASS_STACCATO_PARAM, 0.f, 1.f, 1.f, "Staccato"); 
		configParam(BUTTON_BASS_OCTAVES_PARAM, 0.f, 1.f, 1.f, "Play as Octaves");
							
		configParam(CONTROL_HARMONY_FBM_OCTAVES_PARAM, 1.f, 6.f, 3.f, "");
		configParam(CONTROL_MELODY_FBM_OCTAVES_PARAM, 1.f, 6.f, 3.f, "");
		configParam(CONTROL_ARP_FBM_OCTAVES_PARAM, 1.f, 6.f, 3.f, "");

		configParam(CONTROL_HARMONY_FBM_PERIOD_PARAM, 1.f, 100.f, 60.f, "");
		configParam(CONTROL_MELODY_FBM_PERIOD_PARAM, 1.f, 100.f, 10.f, "");
		configParam(CONTROL_ARP_FBM_PERIOD_PARAM, 1.f, 100.f, 1.f, "");
       	

		configParam(BUTTON_HARMONY_SETSTEP_1_PARAM, 0.f, 1.f, 0.f, "Step 1");
		configParam(BUTTON_HARMONY_SETSTEP_2_PARAM, 0.f, 1.f, 0.f, "Step 2");
		configParam(BUTTON_HARMONY_SETSTEP_3_PARAM, 0.f, 1.f, 0.f, "Step 3");
		configParam(BUTTON_HARMONY_SETSTEP_4_PARAM, 0.f, 1.f, 0.f, "Step 4");
		configParam(BUTTON_HARMONY_SETSTEP_5_PARAM, 0.f, 1.f, 0.f, "Step 5");
		configParam(BUTTON_HARMONY_SETSTEP_6_PARAM, 0.f, 1.f, 0.f, "Step 6");
		configParam(BUTTON_HARMONY_SETSTEP_7_PARAM, 0.f, 1.f, 0.f, "Step 7");
		configParam(BUTTON_HARMONY_SETSTEP_8_PARAM, 0.f, 1.f, 0.f, "Step 8");
		configParam(BUTTON_HARMONY_SETSTEP_9_PARAM, 0.f, 1.f, 0.f, "Step 9");
		configParam(BUTTON_HARMONY_SETSTEP_10_PARAM, 0.f, 1.f, 0.f, "Step 10");
		configParam(BUTTON_HARMONY_SETSTEP_11_PARAM, 0.f, 1.f, 0.f, "Step 11");
		configParam(BUTTON_HARMONY_SETSTEP_12_PARAM, 0.f, 1.f, 0.f, "Step 12");
		configParam(BUTTON_HARMONY_SETSTEP_13_PARAM, 0.f, 1.f, 0.f, "Step 13");
		configParam(BUTTON_HARMONY_SETSTEP_14_PARAM, 0.f, 1.f, 0.f, "Step 14");
		configParam(BUTTON_HARMONY_SETSTEP_15_PARAM, 0.f, 1.f, 0.f, "Step 15");
		configParam(BUTTON_HARMONY_SETSTEP_16_PARAM, 0.f, 1.f, 0.f, "Step 16");

		configParam(BUTTON_CIRCLESTEP_C_PARAM, 0.f, 1.f, 1.f, "C");
		configParam(BUTTON_CIRCLESTEP_G_PARAM, 0.f, 1.f, 0.f, "G");
		configParam(BUTTON_CIRCLESTEP_D_PARAM, 0.f, 1.f, 0.f, "D");
		configParam(BUTTON_CIRCLESTEP_A_PARAM, 0.f, 1.f, 0.f, "A");
		configParam(BUTTON_CIRCLESTEP_E_PARAM, 0.f, 1.f, 0.f, "E");
		configParam(BUTTON_CIRCLESTEP_B_PARAM, 0.f, 1.f, 0.f, "B");
		configParam(BUTTON_CIRCLESTEP_GBFS_PARAM, 0.f, 1.f, 0.f, "Gb/F#");
		configParam(BUTTON_CIRCLESTEP_DB_PARAM, 0.f, 1.f, 0.f, "Db");
		configParam(BUTTON_CIRCLESTEP_AB_PARAM, 0.f, 1.f, 0.f, "Ab");
		configParam(BUTTON_CIRCLESTEP_EB_PARAM, 0.f, 1.f, 0.f, "Eb");
		configParam(BUTTON_CIRCLESTEP_BB_PARAM, 0.f, 1.f, 0.f, "Bb");
		configParam(BUTTON_CIRCLESTEP_F_PARAM, 0.f, 1.f, 0.f, "F");

		configParam(BUTTON_PROG_STEP_PARAM, 0.f, 1.f, 0.f, "Step Harmony");

	}  // end Meander()
	
};  // end of struct Meander

 
struct RootKeySelectLineDisplay : TransparentWidget {
	
	int frame = 0;
	std::shared_ptr<Font> font;
	Meander *module;

	RootKeySelectLineDisplay() {
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/EurostileBold.ttf"));
	}

	void draw(const DrawArgs &ctx) override {

		Vec pos = Vec(18,-11); // this is the offset if any in the passed box position, particularly x indention -7.3=box height
	
		// Background
		NVGcolor backgroundColor = nvgRGB(0x0, 0x0, 0x0);
		NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
		nvgBeginPath(ctx.vg);
		nvgRoundedRect(ctx.vg, 0.0, -22.0, box.size.x, box.size.y, 4.0);
		nvgFillColor(ctx.vg, backgroundColor);
		nvgFill(ctx.vg);
		nvgStrokeWidth(ctx.vg, 1.0);
		nvgStrokeColor(ctx.vg, borderColor);
		nvgStroke(ctx.vg);
	
		if (globalsInitialized)  // global fully initialized if Module!=NULL
		{
			nvgFontSize(ctx.vg,18 );
			nvgFontFaceId(ctx.vg, font->handle);
			nvgTextLetterSpacing(ctx.vg, -1);
			nvgTextAlign(ctx.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
			nvgFillColor(ctx.vg, nvgRGBA(0xff, 0xff, 0x2c, 0xFF));

			nvgText(ctx.vg, pos.x, pos.y, root_key_names[module->root_key], NULL);
		}
	}

};

struct ScaleSelectLineDisplay : TransparentWidget {
	
	int frame = 0;
	std::shared_ptr<Font> font;
	Meander *module;

	ScaleSelectLineDisplay() {
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/EurostileBold.ttf"));
	}

	void draw(const DrawArgs &ctx) override {

		Vec pos = Vec(60,12); 
	
		// Background
		NVGcolor backgroundColor = nvgRGB(0x0, 0x0, 0x0);
		NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
		nvgBeginPath(ctx.vg);
		nvgRoundedRect(ctx.vg, 0.0, 0, box.size.x, box.size.y, 4.0);
		nvgFillColor(ctx.vg, backgroundColor);
		nvgFill(ctx.vg);
		nvgStrokeWidth(ctx.vg, 1.0);
		nvgStrokeColor(ctx.vg, borderColor);
		nvgStroke(ctx.vg);
	 
	 	if (globalsInitialized)  // global fully initialized if Module!=NULL
		{
			nvgFontSize(ctx.vg, 16);
			nvgFontFaceId(ctx.vg, font->handle);
			nvgTextLetterSpacing(ctx.vg, -1);
			nvgTextAlign(ctx.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
			nvgFillColor(ctx.vg, nvgRGBA(0xff, 0xff, 0x2c, 0xFF));
		
			nvgText(ctx.vg, pos.x, pos.y, mode_names[module->mode], NULL);

			// add on the scale notes display out of this box
			nvgFillColor(ctx.vg, nvgRGBA(0x00, 0x0, 0x0, 0xFF));
			char text[128] = {0};
			for (int i=0;i<mode_step_intervals[module->mode][0];++i)
			{
				strcat(text,module->note_desig[module->notes[i]%MAX_NOTES]);  
				strcat(text," ");
			}
			
			nvgText(ctx.vg, pos.x, pos.y+24, text, NULL);
		}
		
	} 

};

////////////////////////////////////
struct BpmDisplayWidget : TransparentWidget {

  float *val = NULL;

  std::shared_ptr<Font> font;

  BpmDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

  void draw(const DrawArgs &args) override {
	    

	// draw the display background even if there is no module
	// Background
	NVGcolor backgroundColor = nvgRGB(0x20, 0x10, 0x10);
	NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
	nvgBeginPath(args.vg);
	nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
	nvgFillColor(args.vg, backgroundColor);
	nvgFill(args.vg);
	nvgStrokeWidth(args.vg, 1.0);
	nvgStrokeColor(args.vg, borderColor);
	nvgStroke(args.vg);

	// if val is null (Module null)
	if (!val) { 
      return;
    }

	nvgFontSize(args.vg, 36);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextLetterSpacing(args.vg, 2.5);

	std::string to_display = std::to_string((int)(*val));
	while(to_display.length()<3) to_display = ' ' + to_display;

	Vec textPos = Vec(6.0f, 30.0f);

	NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
	nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
	nvgText(args.vg, textPos.x, textPos.y, "~~~", NULL);

	textColor = nvgRGB(0xda, 0xe9, 0x29);

	nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
	nvgText(args.vg, textPos.x, textPos.y, "\\\\\\", NULL);

	textColor = nvgRGB(0xff, 0xff, 0x2c);
	nvgFillColor(args.vg, textColor);
	nvgText(args.vg, textPos.x, textPos.y, to_display.c_str(), NULL);
	
  }
};
////////////////////////////////////
struct SigDisplayWidget : TransparentWidget {

  int *value = NULL;
  std::shared_ptr<Font> font;

  SigDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
    
    
  };

  void draw(const DrawArgs &args) override {
	
 

	// Display Background is now drawn on the svg panel, even if Module is null (value=null)
    NVGcolor backgroundColor = nvgRGB(0x20, 0x10, 0x10);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(args.vg, backgroundColor);
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.0);
    nvgStrokeColor(args.vg, borderColor);
    nvgStroke(args.vg); 

	 if (!value) {
      return;
    }
         
    // text 
 	nvgFontSize(args.vg, 20);  
    nvgFontFaceId(args.vg, font->handle);
    nvgTextLetterSpacing(args.vg, 2.2);

    std::stringstream to_display;   
    to_display << std::setw(2) << *value;

	Vec textPos = Vec(-2.0f, 17.0f);   // this is a relative offset within the box
	 
    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "\\\\", NULL);
    
   
 	textColor = nvgRGBA(0xff, 0xff, 0x2c, 0xff);
    nvgFillColor(args.vg, textColor);
    nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
  }
};
//////////////////////////////////

struct RSLabelCentered : LedDisplay {
	std::shared_ptr<Font> font;
	int fontSize;
	std::string text;
	NVGcolor color;

//	RSLabelCentered(int x, int y, const char* str = "", int fontSize = 10, const NVGcolor& colour = COLOR_RS_GREY) {
	RSLabelCentered(int x, int y, const char* str = "", int fontSize = 10, const NVGcolor& colour = nvgRGB(0x00, 0x00, 0x00)) {
	//	font = APP->window->loadFont(asset::plugin(pluginInstance, "res/fonts/Ubuntu Condensed 400.ttf"));  // can't load if this is the textfont
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSansMono.ttf"));  // load a not textfont
		this->fontSize = fontSize;
		box.pos = Vec(x, y);
		text = str;
		color = colour;
	}

	void draw(const DrawArgs &args) override {
		if(font->handle >= 0) {
			bndSetFont(font->handle);

			nvgFontSize(args.vg, fontSize);
			nvgFontFaceId(args.vg, font->handle);
			nvgTextLetterSpacing(args.vg, 0);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER);

			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, color);
			nvgText(args.vg, 0, 0, text.c_str(), NULL);
			nvgStroke(args.vg);

			bndSetFont(APP->window->uiFont->handle);
		}
	}
};

///////////////////////////////
 
 
struct MeanderWidget : ModuleWidget 
{
	rack::math::Rect  ParameterRect[MAX_PARAMS];  // warning, don't exceed the dimension
    rack::math::Rect  InportRect[MAX_INPORTS];  // warning, don't exceed the dimension
    rack::math::Rect  OutportRect[MAX_OUTPORTS];  // warning, don't exceed the dimension
 
	ParamWidget* paramWidgets[Meander::NUM_PARAMS]={0};  // keep track of all ParamWidgets as they are created so they can be moved around later  by the enum parmam ID
	LightWidget* lightWidgets[Meander::NUM_LIGHTS]={0};  // keep track of all LightWidgets as they are created so they can be moved around later  by the enum parmam ID

	PortWidget* outPortWidgets[Meander::NUM_OUTPUTS]={0};  // keep track of all output TPortWidgets as they are created so they can be moved around later  by the enum parmam ID
	PortWidget* inPortWidgets[Meander::NUM_INPUTS]={0};  // keep track of all output TPortWidgets as they are created so they can be moved around later  by the enum parmam ID

	struct CircleOf5thsDisplay : TransparentWidget 
	{
		Meander *module;
		rack::math::Rect*  ParameterRectLocal;   // warning, don't exceed the dimension
		rack::math::Rect*  InportRectLocal; 	 // warning, don't exceed the dimension
		rack::math::Rect*  OutportRectLocal;     // warning, don't exceed the dimension
						
		int frame = 0;
		std::shared_ptr<Font> textfont;
		std::shared_ptr<Font> musicfont; 

		CircleOf5thsDisplay()  
		{
		//	textfont = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSansMono.ttf"));
			textfont = APP->window->loadFont(asset::plugin(pluginInstance, "res/Ubuntu Condensed 400.ttf"));
	    	musicfont = APP->window->loadFont(asset::plugin(pluginInstance, "res/Musisync-KVLZ.ttf"));
		}
 
	 	  

		void DrawCircle5ths(const DrawArgs &args, int root_key) 
		{
			if (doDebug) DEBUG("DrawCircle5ths()");
			
			for (int i=0; i<MAX_CIRCLE_STATIONS; ++i)
			{
					// draw root_key annulus sector

					int relativeCirclePosition = ((i - module->circle_root_key + module->mode)+12) % MAX_CIRCLE_STATIONS;
					if (doDebug) DEBUG("\nrelativeCirclePosition-1=%d", relativeCirclePosition);

					nvgBeginPath(args.vg);
					float opacity = 128;

					nvgStrokeColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
					nvgStrokeWidth(args.vg, 2);
				
					if((relativeCirclePosition == 0)||(relativeCirclePosition == 1)||(relativeCirclePosition == 2)) 
						nvgFillColor(args.vg, nvgRGBA(0xff, 0x20, 0x20, (int)opacity));  // reddish
					else
					if((relativeCirclePosition == 3)||(relativeCirclePosition == 4)||(relativeCirclePosition == 5)) 
						nvgFillColor(args.vg, nvgRGBA(0x20, 0x20, 0xff, (int)opacity));  //bluish
					else
					if(relativeCirclePosition == 6)
						nvgFillColor(args.vg, nvgRGBA(0x20, 0xff, 0x20, (int)opacity));  // greenish
					else	
						nvgFillColor(args.vg, nvgRGBA(0x20, 0x20, 0x20, (int)opacity));  // grayish
				
					nvgArc(args.vg,module->theCircleOf5ths.CircleCenter.x,module->theCircleOf5ths.CircleCenter.y,module->theCircleOf5ths.MiddleCircleRadius,module->theCircleOf5ths.Circle5ths[i].startDegree,module->theCircleOf5ths.Circle5ths[i].endDegree,NVG_CW);
					nvgLineTo(args.vg,module->theCircleOf5ths.Circle5ths[i].pt3.x,module->theCircleOf5ths.Circle5ths[i].pt3.y);
					nvgArc(args.vg,module->theCircleOf5ths.CircleCenter.x,module->theCircleOf5ths.CircleCenter.y,module->theCircleOf5ths.InnerCircleRadius,module->theCircleOf5ths.Circle5ths[i].endDegree,module->theCircleOf5ths.Circle5ths[i].startDegree,NVG_CCW);
					nvgLineTo(args.vg,module->theCircleOf5ths.Circle5ths[i].pt2.x,module->theCircleOf5ths.Circle5ths[i].pt2.y);
								
					nvgFill(args.vg);
					nvgStroke(args.vg);
					
					nvgClosePath(args.vg);	
								
					// draw text
					nvgFontSize(args.vg, 12);
					nvgFontFaceId(args.vg, textfont->handle);	
					nvgTextLetterSpacing(args.vg, -1);
					nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
					if (doDebug) DEBUG("radialDirection= %.3f %.3f", module->theCircleOf5ths.Circle5ths[i].radialDirection.x, module->theCircleOf5ths.Circle5ths[i].radialDirection.y);
					Vec TextPosition=module->theCircleOf5ths.CircleCenter.plus(module->theCircleOf5ths.Circle5ths[i].radialDirection.mult(module->theCircleOf5ths.MiddleCircleRadius*.93f));
					nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
					nvgText(args.vg, TextPosition.x, TextPosition.y, CircleNoteNames[i], NULL);

			}		
		};

		void DrawDegreesSemicircle(const DrawArgs &args, int root_key) 
		{
			if (doDebug) DEBUG("DrawDegreesSemicircle()");
			int chord_type=0;

			for (int i=0; i<MAX_HARMONIC_DEGREES; ++i)
			{
				// draw degree annulus sector

					nvgBeginPath(args.vg);
					float opacity = 128;

					nvgStrokeColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
					nvgStrokeWidth(args.vg, 2);

					chord_type=module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].chordType;

					if (false)
					{
						if (chord_type==0)  // major
							nvgFillColor(args.vg, nvgRGBA(0xff, 0x20, 0x20, (int)opacity));  // reddish
						else
						if (chord_type==1)  //minor
							nvgFillColor(args.vg, nvgRGBA(0x20, 0x20, 0xff, (int)opacity));  //bluish
						else
						if (chord_type==6)  // diminished
							nvgFillColor(args.vg, nvgRGBA(0x20, 0xff, 0x20, (int)opacity));  // greenish
					}
					else
						nvgFillColor(args.vg, nvgRGBA(0xf9, 0xf9, 0x20, (int)opacity));  // yellowish
						
					nvgArc(args.vg,module->theCircleOf5ths.CircleCenter.x,module->theCircleOf5ths.CircleCenter.y,module->theCircleOf5ths.OuterCircleRadius,module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].startDegree,module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].endDegree,NVG_CW);
					nvgLineTo(args.vg,module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].pt3.x,module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].pt3.y);
					nvgArc(args.vg,module->theCircleOf5ths.CircleCenter.x,module->theCircleOf5ths.CircleCenter.y,module->theCircleOf5ths.MiddleCircleRadius,module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].endDegree,module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].startDegree,NVG_CCW);
					nvgLineTo(args.vg,module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].pt2.x,module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].pt2.y);
					
					nvgFill(args.vg);
					nvgStroke(args.vg);
					
					nvgClosePath(args.vg);	
								
					// draw text
					nvgFontSize(args.vg, 10);
					nvgFontFaceId(args.vg, textfont->handle);	
					nvgTextLetterSpacing(args.vg, -1); // as close as possible
					nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
					const char *text;
				
					
					if (chord_type==0) // major
						text = circle_of_fifths_degrees_UC[(i - module->theCircleOf5ths.theDegreeSemiCircle.RootKeyCircle5thsPosition+7)%7];
						else
					if ((chord_type==1)||(chord_type==6)) // minor or diminished
						text = circle_of_fifths_degrees_LC[(i - module->theCircleOf5ths.theDegreeSemiCircle.RootKeyCircle5thsPosition+7)%7];
					
					if (doDebug) DEBUG("radialDirection= %.3f %.3f", module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].radialDirection.x, module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].radialDirection.y);
					Vec TextPosition=module->theCircleOf5ths.CircleCenter.plus(module->theCircleOf5ths.theDegreeSemiCircle.degreeElements[i].radialDirection.mult(module->theCircleOf5ths.OuterCircleRadius*.92f));
					nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
					nvgText(args.vg, TextPosition.x, TextPosition.y, text, NULL);
					if (i==6) // draw diminished
					{
						Vec TextPositionBdim=Vec(TextPosition.x+9, TextPosition.y-4);
						if (doDebug) DEBUG("o");
						nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
						nvgFontSize(args.vg, 8);
						nvgText(args.vg, TextPositionBdim.x, TextPositionBdim.y, "o", NULL);
					}

			}	
			
		};

		Vec convertSVGtoNVG(float x, float y, float w, float h)
		{
			Vec nvgPos=mm2px(Vec(x+(w/2.0), (128.93-y-(h/2.0))));  // this writes in the center
			return nvgPos;
		}

		void drawHarmonyControlParamLine(const DrawArgs &args, Vec paramControlPos, const char* label, float value, int valueDecimalPoints)
		{
			Vec displayRectPos= paramControlPos.plus(Vec(128, 0)); 
			nvgBeginPath(args.vg);

			if (valueDecimalPoints>=0)
				nvgRoundedRect(args.vg, displayRectPos.x,displayRectPos.y, 45.f, 20.f, 4.f);
			
			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
			nvgFontSize(args.vg, 14);
			nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
			nvgText(args.vg, paramControlPos.x+20, paramControlPos.y+10, label, NULL);

			nvgFillColor(args.vg, nvgRGBA( 0x2f,  0x27, 0x0a, 0xff));
			nvgFill(args.vg);
			nvgStroke(args.vg);
			nvgFillColor(args.vg, nvgRGBA(0xFF, 0xFF, 0x2C, 0xff));
			nvgFontSize(args.vg, 17);
			if (valueDecimalPoints>=0)
			{
				char text[32];
				nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
				if (valueDecimalPoints==0)
					snprintf(text, sizeof(text), "%d", (int)value);
				else
				if (valueDecimalPoints==1)
					snprintf(text, sizeof(text), "%.1lf", value);
				else
				if (valueDecimalPoints==2)
					snprintf(text, sizeof(text), "%.2lf", value);
				nvgText(args.vg, displayRectPos.x+25, displayRectPos.y+10, text, NULL);
			}
			
		}

		void drawMelodyControlParamLine(const DrawArgs &args, Vec paramControlPos, const char* label, float value, int valueDecimalPoints)
		{
			Vec displayRectPos= paramControlPos.plus(Vec(115, 0));
			nvgBeginPath(args.vg);

			if (valueDecimalPoints>=0)
				nvgRoundedRect(args.vg, displayRectPos.x,displayRectPos.y, 45.f, 20.f, 4.f);
			
			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
			nvgFontSize(args.vg, 14);
			nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
			nvgText(args.vg, paramControlPos.x+20, paramControlPos.y+10, label, NULL);

			nvgFillColor(args.vg, nvgRGBA( 0x2f,  0x27, 0x0a, 0xff));
			nvgFill(args.vg);
			nvgStroke(args.vg);
			nvgFillColor(args.vg, nvgRGBA(0xFF, 0xFF, 0x2C, 0xff));
			nvgFontSize(args.vg, 17);
			if (valueDecimalPoints>=0)
			{
				char text[32];
				nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
				if (valueDecimalPoints==0)
					snprintf(text, sizeof(text), "%d", (int)value);
				else
				if (valueDecimalPoints==1)
					snprintf(text, sizeof(text), "%.1lf", value);
				else
				if (valueDecimalPoints==2)
					snprintf(text, sizeof(text), "%.2lf", value);
				nvgText(args.vg, displayRectPos.x+25, displayRectPos.y+10, text, NULL);
			}
			
		}

		void drawBassControlParamLine(const DrawArgs &args, Vec paramControlPos, const char* label, float value, int valueDecimalPoints)
		{
			Vec displayRectPos= paramControlPos.plus(Vec(85, 0));
			nvgBeginPath(args.vg);

			if (valueDecimalPoints>=0)
				nvgRoundedRect(args.vg, displayRectPos.x,displayRectPos.y, 45.f, 20.f, 4.f);

			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
			nvgFontSize(args.vg, 14);
			nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
			nvgText(args.vg, paramControlPos.x+20, paramControlPos.y+10, label, NULL);

			nvgFillColor(args.vg, nvgRGBA( 0x2f,  0x27, 0x0a, 0xff));
			nvgFill(args.vg);
			nvgStroke(args.vg);
			nvgFillColor(args.vg, nvgRGBA(0xFF, 0xFF, 0x2C, 0xff));
			nvgFontSize(args.vg, 17);
			if (valueDecimalPoints>=0)
			{
				char text[32];
				nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
				if (valueDecimalPoints==0)
					snprintf(text, sizeof(text), "%d", (int)value);
				else
				if (valueDecimalPoints==1)
					snprintf(text, sizeof(text), "%.1lf", value);
				else
				if (valueDecimalPoints==2)
					snprintf(text, sizeof(text), "%.2lf", value);
				nvgText(args.vg, displayRectPos.x+25, displayRectPos.y+10, text, NULL);
			}
			
		}

		void drawfBmControlParamLine(const DrawArgs &args, Vec paramControlPos, const char* label, float value, int valueDecimalPoints)
		{
			Vec displayRectPos= paramControlPos.plus(Vec(105, 0));
			nvgBeginPath(args.vg);

			if (valueDecimalPoints>=0)
				nvgRoundedRect(args.vg, displayRectPos.x,displayRectPos.y, 45.f, 20.f, 4.f);
			
			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
			nvgFontSize(args.vg, 14);
			nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
			nvgText(args.vg, paramControlPos.x+20, paramControlPos.y+10, label, NULL);

			nvgFillColor(args.vg, nvgRGBA( 0x2f,  0x27, 0x0a, 0xff));
			nvgFill(args.vg);
			nvgStroke(args.vg);
			nvgFillColor(args.vg, nvgRGBA(0xFF, 0xFF, 0x2C, 0xff));
			nvgFontSize(args.vg, 17);
			if (valueDecimalPoints>=0)
			{
				char text[32];
				nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
				if (valueDecimalPoints==0)
					snprintf(text, sizeof(text), "%d", (int)value);
				else
				if (valueDecimalPoints==1)
					snprintf(text, sizeof(text), "%.1lf", value);
				else
				if (valueDecimalPoints==2)
					snprintf(text, sizeof(text), "%.2lf", value);
				nvgText(args.vg, displayRectPos.x+25, displayRectPos.y+10, text, NULL);
			}
			
		}

		void drawLabelAbove(const DrawArgs &args, Rect rect, const char* label, float fontSize)  
		{
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, nvgRGBA( 0x0,  0x0, 0x0, 0xff));
			nvgFontSize(args.vg, fontSize);
			nvgFontFaceId(args.vg, textfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
		//	nvgText(args.vg, rect.pos.x+rect.size.x/2.,rect.pos.y-4, label, NULL);
			nvgText(args.vg, rect.pos.x+rect.size.x/2.,rect.pos.y-8, label, NULL);
		}

		void drawLabelRight(const DrawArgs &args, Rect rect, const char* label)  
		{
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, nvgRGBA( 0x0,  0x0, 0x0, 0xff));
			nvgFontSize(args.vg, 14);
			nvgFontFaceId(args.vg, textfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
			nvgText(args.vg, rect.pos.x+rect.size.x+2, rect.pos.y+rect.size.y/2., label, NULL);
		}

		void drawLabelLeft(const DrawArgs &args, Rect rect, const char* label, float xoffset)  
		{
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, nvgRGBA( 0x0,  0x0, 0x0, 0xff));
			nvgFontSize(args.vg, 14);
			nvgFontFaceId(args.vg, textfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
			nvgText(args.vg, rect.pos.x-rect.size.x-2+xoffset, rect.pos.y+rect.size.y/2., label, NULL);
		}

		void drawLabelOffset(const DrawArgs &args, Rect rect, const char* label, float xoffset, float yoffset)  
		{
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, nvgRGBA( 0x0,  0x0, 0x0, 0xff));
			nvgFontSize(args.vg, 14);
			nvgFontFaceId(args.vg, textfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
			nvgText(args.vg, rect.pos.x+xoffset, rect.pos.y+yoffset, label, NULL);
		}

		void drawOutport(const DrawArgs &args, Vec OutportPos, const char* label, float value, int valueDecimalPoints)  // test draw a rounded corner rect  for jack border testing
		{
			Vec displayRectPos= OutportPos.plus(Vec(-3, -15));
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, displayRectPos.x,displayRectPos.y, 30.f, 43.f, 3.f);
			nvgFillColor(args.vg, nvgRGBA( 0x0,  0x0, 0x0, 0xff));
			nvgFill(args.vg);
			nvgFontSize(args.vg, 10);
			nvgFontFaceId(args.vg, textfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xFF));
			nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
			nvgText(args.vg, OutportPos.x+12, OutportPos.y-8, label, NULL);
		}

		void drawGrid(const DrawArgs &args)
		{
			
				float panelWidth=mm2px(406);
				float panelHeight=mm2px(129);
				if (doDebug) DEBUG("panel size (WxH) in px=%.2f x %.2f", panelWidth, panelHeight);  // panel size (WxH) in px=1198.82 x 380.91
				int gridWidthDivisions=(int)(panelWidth/10.0);  // 112
				int gridHeightDivisions=(int)(panelHeight/10.0); // 38
				float gridWidth=10.0;
				float gridHeight=10.0;

				
				nvgStrokeColor(args.vg, nvgRGB(0, 0, 0));
				nvgStrokeWidth(args.vg, 1.0);
				
				for (int x=0; x<gridWidthDivisions; ++x)
				{
					if (x%10==0)
					{
						nvgStrokeWidth(args.vg, 2.0);
						nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0xff, 0x80));
					}
					else
					{
						nvgStrokeWidth(args.vg, 1.0);
						nvgStrokeColor(args.vg, nvgRGBA(0xff, 0, 0, 0x80));
					}

					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, x*gridWidth, 0);
					nvgLineTo(args.vg, x*gridWidth,  gridHeightDivisions*gridHeight);
					nvgStroke(args.vg);
				}
				for (int y=0; y<gridHeightDivisions; ++y)
				{
					if (y%10==0)
					{
						nvgStrokeWidth(args.vg, 2.0);
						nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0xff, 0x80));
					}
					else
					{
						nvgStrokeWidth(args.vg, 1.0);
						nvgStrokeColor(args.vg, nvgRGBA(0xff, 0, 0, 0x80));
					}

					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, 0, y*gridHeight);
					nvgLineTo(args.vg, gridWidthDivisions*gridWidth, y*gridHeight);
					nvgStroke(args.vg);
				}
		}

		void updatePanel(const DrawArgs &args)
		{
					 
			if (true)  // Harmony  position a paramwidget  can't access paramWidgets here  
			{
				nvgBeginPath(args.vg);
				nvgRoundedRect(args.vg, 484.f,25.f, 196.f, 353.f, 4.f);
				nvgFillColor(args.vg, nvgRGBA( 0xe6,  0xe6, 0xe6, 0xff));
				nvgFill(args.vg);
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.5);
				nvgStroke(args.vg); 
				
				Vec pos;
				Vec displayRectPos;
				Vec paramControlPos;
				char text[128];
				char labeltext[128];
				nvgFontSize(args.vg, 17);
				nvgFontFaceId(args.vg, textfont->handle);
				nvgTextLetterSpacing(args.vg, -1);
				nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.0); 

				// draw set steps text 
				for(int i = 0; i<MAX_STEPS;++i) 
				{
					if (i==0)
					{
						sprintf(labeltext, "Set Step");
						drawLabelAbove(args, ParameterRectLocal[Meander::BUTTON_HARMONY_SETSTEP_1_PARAM+i], labeltext, 15.);  
					}
					sprintf(labeltext, "%d", i+1);
					drawLabelLeft(args,ParameterRectLocal[Meander::BUTTON_HARMONY_SETSTEP_1_PARAM+i], labeltext, 0.);  
				}

              

				//***************
				// update harmony panel begin
		
				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_HARMONY_PARAM].pos, "Harmony Enable", 0, -1);
				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_VOLUME_PARAM].pos, "Volume (0-10.0)", module->theMeanderState.theHarmonyParms.volume, 1);
						    
				snprintf(labeltext, sizeof(labeltext), "Steps (%d-%d)", module->theActiveHarmonyType.min_steps, module->theActiveHarmonyType.max_steps);
				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_STEPS_PARAM].pos, labeltext, (float)module->theActiveHarmonyType.num_harmony_steps, 0);
				
				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_TARGETOCTAVE_PARAM].pos, "Target Oct.(1-6)", module->theMeanderState.theHarmonyParms.target_octave, 0);

				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_ALPHA_PARAM].pos, "Variability (0-1)", module->theMeanderState.theHarmonyParms.alpha, 2);

				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_RANGE_PARAM].pos, "+-Octave Range (0-3)", module->theMeanderState.theHarmonyParms.note_octave_range, 2);

				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_DIVISOR_PARAM].pos, "Notes on 1/", module->theMeanderState.theHarmonyParms.note_length_divisor, 0);

				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_HARMONY_ALL7THS_PARAM].pos, "~Nice 7ths", 0, -1);

				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_HARMONY_V7THS_PARAM].pos, "V 7ths", 0, -1);

				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_HARMONY_STACCATO_PARAM].pos, "Staccato", 0, -1);

				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONYPRESETS_PARAM].pos, "Presets", 0, -1);
				
				drawHarmonyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_PROG_STEP_PARAM].pos, " STEP", 0, -1);
 
				//  do the progression displays
				pos =ParameterRectLocal[Meander::CONTROL_HARMONYPRESETS_PARAM].pos.plus(Vec(-20,20));
							
				nvgBeginPath(args.vg);
				nvgFillColor(args.vg, nvgRGBA( 0x2f,  0x27, 0x0a, 0xff));
				nvgRoundedRect(args.vg, pos.x,pos.y, 195.f, 20.f, 4.f);
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.0);
				nvgFill(args.vg);
				nvgStroke(args.vg);

				
				if (globalsInitialized)  // global fully initialized if Module!=NULL
				{
					nvgBeginPath(args.vg);
					nvgFontSize(args.vg, 14);
					nvgFontFaceId(args.vg, textfont->handle);
					nvgTextLetterSpacing(args.vg, -1);
					nvgFillColor(args.vg, nvgRGBA(0xFF, 0xFF, 0x2C, 0xFF));
					snprintf(text, sizeof(text), "#%d:  %s", module->harmony_type, module->theActiveHarmonyType.harmony_type_desc);
					nvgText(args.vg, pos.x+5, pos.y+10, text, NULL);
				}
				pos = pos.plus(Vec(0,20));

							
				nvgBeginPath(args.vg);
				nvgFillColor(args.vg, nvgRGBA( 0x2f,  0x27, 0x0a, 0xff));
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.0);
				nvgRoundedRect(args.vg, pos.x,pos.y, 195.f, 20.f, 4.f);
				nvgFill(args.vg);
				nvgStroke(args.vg);

				if (globalsInitialized)  // global fully initialized if Module!=NULL
				{
					nvgBeginPath(args.vg);
					nvgFontSize(args.vg, 12);
					nvgFillColor(args.vg, nvgRGBA(0xFF, 0xFF, 0x2C, 0xFF));
					snprintf(text, sizeof(text), "%s           ",  module->theActiveHarmonyType.harmony_degrees_desc);
					nvgText(args.vg, pos.x+5, pos.y+10, text, NULL);
				}
								
			}

			if  (true)  // update melody control display
			{

				nvgBeginPath(args.vg);
				nvgRoundedRect(args.vg, 680.f,25.f, 187.f, 353.f, 4.f);
				nvgFillColor(args.vg, nvgRGBA( 0xe6,  0xe6, 0xe6, 0xff));
				nvgFill(args.vg);
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.5);
				nvgStroke(args.vg); 

				
				Vec pos;
				Vec displayRectPos;
				Vec paramControlPos;
				char text[128];
				char labeltext[128];
				nvgFontSize(args.vg, 17);
				nvgFontFaceId(args.vg, textfont->handle);
				nvgTextLetterSpacing(args.vg, -1);
				nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.0);
				//***************

				// update melody panel begin
		
				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_MELODY_PARAM].pos, "Melody Enable", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_MELODY_PARAM].pos.plus(Vec(92,-8)), "Degree(1-7)", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_MELODY_PARAM].pos.plus(Vec(92,6)), "Gate", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_MELODY_CHORDAL_PARAM].pos, "Chordal", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_MELODY_SCALER_PARAM].pos, "Scaler", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_MELODY_VOLUME_PARAM].pos, "Volume (0-10.0)", module->theMeanderState.theMelodyParms.volume, 1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_MELODY_DESTUTTER_PARAM].pos, "Hold tied", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM].pos, "Notes on 1/", module->theMeanderState.theMelodyParms.note_length_divisor, 0);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_MELODY_TARGETOCTAVE_PARAM].pos, "Target Oct.(1-6)", module->theMeanderState.theMelodyParms.target_octave, 0);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_MELODY_ALPHA_PARAM].pos, "Variability (0-1)", module->theMeanderState.theMelodyParms.alpha, 2);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_MELODY_RANGE_PARAM].pos, "+-Octave Range (0-3)", module->theMeanderState.theMelodyParms.note_octave_range, 2);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_MELODY_STACCATO_PARAM].pos, "Staccato", 0, -1);

				// draw division line
				pos = ParameterRectLocal[Meander::BUTTON_ENABLE_ARP_PARAM].pos.plus(Vec(-20,-2));
				nvgMoveTo(args.vg, 
				pos.x, pos.y);
				pos=pos.plus(Vec(190,0));
				nvgLineTo(args.vg, pos.x, pos.y);  
				nvgStrokeColor(args.vg, nvgRGB(0, 0, 0));
				nvgStrokeWidth(args.vg, 1.0);
				nvgStroke(args.vg);
				//

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_ARP_PARAM].pos, "Arp Enable", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_ARP_COUNT_PARAM].pos, "Count (0-31)", module->theMeanderState.theArpParms.count, 0);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_ARP_INCREMENT_PARAM].pos, "Notes on 1/", module->theMeanderState.theArpParms.note_length_divisor, 0);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_ARP_DECAY_PARAM].pos, "Decay (0-1.0)", module->theMeanderState.theArpParms.decay, 2);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_ARP_CHORDAL_PARAM].pos, "Chordal", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_ARP_SCALER_PARAM].pos, "Scaler", 0, -1);

				drawMelodyControlParamLine(args,ParameterRectLocal[Meander::CONTROL_ARP_PATTERN_PARAM].pos, "Pattern (-3-+3", module->theMeanderState.theArpParms.pattern, -1);

				pos =ParameterRectLocal[Meander::CONTROL_ARP_PATTERN_PARAM].pos.plus(Vec(102,0));
							
				nvgBeginPath(args.vg);
			
				nvgFillColor(args.vg, nvgRGBA( 0x2f,  0x27, 0x0a, 0xff));
				nvgRoundedRect(args.vg, pos.x,pos.y, 60.f, 20.f, 4.f);
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.0);
				nvgFill(args.vg);
				nvgStroke(args.vg);

				nvgFontSize(args.vg, 17);
				nvgFillColor(args.vg, nvgRGBA(0xFF, 0xFF, 0x2C, 0xFF));
				if (module->theMeanderState.theArpParms.pattern==0)
					snprintf(text, sizeof(text), "%d: 0-echo", module->theMeanderState.theArpParms.pattern);
				else
				if (module->theMeanderState.theArpParms.pattern==1)
					snprintf(text, sizeof(text), "%d: +1", module->theMeanderState.theArpParms.pattern);
				else
				if (module->theMeanderState.theArpParms.pattern==2)
					snprintf(text, sizeof(text), "%d: +1,-1", module->theMeanderState.theArpParms.pattern);
				else
				if (module->theMeanderState.theArpParms.pattern==3)
					snprintf(text, sizeof(text), "%d: +2", module->theMeanderState.theArpParms.pattern);
				else
				if (module->theMeanderState.theArpParms.pattern==-1)
					snprintf(text, sizeof(text), "%d: -1", module->theMeanderState.theArpParms.pattern);
				else
				if (module->theMeanderState.theArpParms.pattern==-2)
					snprintf(text, sizeof(text), "%d: -1,+1", module->theMeanderState.theArpParms.pattern);
				else
				if (module->theMeanderState.theArpParms.pattern==-3)
					snprintf(text, sizeof(text), "%d: -2", module->theMeanderState.theArpParms.pattern);
				else
					snprintf(text, sizeof(text), "%s", "      ");  // since text is used above, needs to be cleared in fallthrough case
				
				nvgText(args.vg, pos.x+10, pos.y+10, text, NULL);

				

			}

			if (true)  // update bass panel
			{
				nvgBeginPath(args.vg);
				nvgRoundedRect(args.vg, 869.f, 120.f, 155.f, 258.f, 4.f);
				nvgFillColor(args.vg, nvgRGBA( 0xe6,  0xe6, 0xe6, 0xff));
				nvgFill(args.vg);
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.5);
				nvgStroke(args.vg); 

				
				Vec pos;
				Vec displayRectPos;
				Vec paramControlPos;
				char labeltext[128];
				nvgFontSize(args.vg, 17);
				nvgFontFaceId(args.vg, textfont->handle);
				nvgTextLetterSpacing(args.vg, -1);
				nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.0);
				
		
				drawBassControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_BASS_PARAM].pos, "Bass Enable", 0, -1);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::CONTROL_BASS_VOLUME_PARAM].pos, "Volume (0-10)", module->theMeanderState.theBassParms.volume, 1);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::CONTROL_BASS_TARGETOCTAVE_PARAM].pos, "Target Oct.(1-6)", module->theMeanderState.theBassParms.target_octave, 0);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::CONTROL_BASS_DIVISOR_PARAM].pos, "Notes on 1/", module->theMeanderState.theBassParms.note_length_divisor, 0);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::BUTTON_ENABLE_BASS_STACCATO_PARAM].pos, "Staccato", module->theMeanderState.theBassParms.enable_staccato, -1);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::BUTTON_BASS_ACCENT_PARAM].pos, "Accent", module->theMeanderState.theBassParms.accent, -1);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::BUTTON_BASS_SYNCOPATE_PARAM].pos, "Syncopate", module->theMeanderState.theBassParms.syncopate, -1);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::BUTTON_BASS_SHUFFLE_PARAM].pos, "Shuffle", module->theMeanderState.theBassParms.shuffle, -1);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::BUTTON_BASS_OCTAVES_PARAM].pos, "Octaves", module->theMeanderState.theBassParms.octave_enabled, -1);



			}

			if (true)  // update fBm panel
			{
				nvgBeginPath(args.vg);
				nvgRoundedRect(args.vg, 1025.f, 120.f, 171.f, 258.f, 4.f);
				nvgFillColor(args.vg, nvgRGBA( 0xe6,  0xe6, 0xe6, 0xff));
				nvgFill(args.vg);
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.5);
				nvgStroke(args.vg); 

				
				Vec pos;
				Vec displayRectPos;
				Vec paramControlPos;
				char labeltext[128];
				nvgFontSize(args.vg, 17);
				nvgFontFaceId(args.vg, textfont->handle);
				nvgTextLetterSpacing(args.vg, -1);
				nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgStrokeWidth(args.vg, 2.0);

				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM].pos.plus(Vec(30,-25)), "fBm 1/f Noise", 0, -1);
				
				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM].pos.plus(Vec(37,-13)), "Harmony", 0, -1);
		
				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM].pos, "Octaves (1-6)", module->theMeanderState.theHarmonyParms.noctaves, 0);

				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_HARMONY_FBM_PERIOD_PARAM].pos, "Period Sec. (1-100)", module->theMeanderState.theHarmonyParms.period, 1);

				drawBassControlParamLine(args,ParameterRectLocal[Meander::CONTROL_MELODY_FBM_OCTAVES_PARAM].pos.plus(Vec(41,-13)), "Melody", 0, -1);

				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_MELODY_FBM_OCTAVES_PARAM].pos, "Octaves (1-6)", module->theMeanderState.theMelodyParms.noctaves, 0);

				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_MELODY_FBM_PERIOD_PARAM].pos, "Period Sec. (1-100)", module->theMeanderState.theMelodyParms.period, 1);

				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_ARP_FBM_OCTAVES_PARAM].pos.plus(Vec(47,-13)), "32nds", 0, -1);

				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_ARP_FBM_OCTAVES_PARAM].pos, "Octaves (1-6)", module->theMeanderState.theArpParms.noctaves, 0);

				drawfBmControlParamLine(args,ParameterRectLocal[Meander::CONTROL_ARP_FBM_PERIOD_PARAM].pos, "Period Sec. (1-100)", module->theMeanderState.theArpParms.period, 1);

				
			} 

			if (true)  // draw rounded corner rects  for input jacks border 
			{
				drawLabelAbove(args,ParameterRectLocal[Meander::BUTTON_RUN_PARAM], "RUN", 12.);

				drawOutport(args, OutportRectLocal[Meander::OUT_RUN_OUT].pos, "Out", 0, 1);
				
				drawLabelAbove(args,ParameterRectLocal[Meander::BUTTON_RESET_PARAM], "RESET", 12.);
				
				drawOutport(args, OutportRectLocal[Meander::OUT_RESET_OUT].pos, "Out", 0, 1);
			
				drawLabelAbove(args,ParameterRectLocal[Meander::CONTROL_TEMPOBPM_PARAM], "BPM", 12.);

				drawLabelRight(args,ParameterRectLocal[Meander::CONTROL_TIMESIGNATURETOP_PARAM], "Time Sig Top");

				drawLabelRight(args,ParameterRectLocal[Meander::CONTROL_TIMESIGNATUREBOTTOM_PARAM], "Time Sig Bottom");

				drawLabelRight(args,ParameterRectLocal[Meander::CONTROL_ROOT_KEY_PARAM], "Root (~Key)");

				drawLabelRight(args,ParameterRectLocal[Meander::CONTROL_SCALE_PARAM], "Mode");

				drawLabelOffset(args, InportRectLocal[Meander::IN_CLOCK_EXT_CV], "        8x BPM Clock", -14., -11.); 
			
				// drawLabelLeft(args, OutportRectLocal[Meander::OUT_EXT_POLY_SCALE_OUTPUT], "Poly Ext. Scale", -40.);

				// drawOutport(args, OutportRectLocal[Meander::OUT_EXT_POLY_SCALE_OUTPUT].pos, "Out", 0, 1);
											
			}

			if (true)  // draw rounded corner rects  for output jacks border 
			{
				drawOutport(args, OutportRectLocal[Meander::OUT_HARMONY_CV_OUTPUT].pos, "1V/Oct", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_HARMONY_NOTE2_OUTPUT].pos, "Note 2", 0, 1);				
				drawOutport(args, OutportRectLocal[Meander::OUT_HARMONY_NOTE3_OUTPUT].pos, "Note 3", 0, 1);				
				drawOutport(args, OutportRectLocal[Meander::OUT_HARMONY_NOTE4_OUTPUT].pos, "Note 4", 0, 1);				

				drawOutport(args, OutportRectLocal[Meander::OUT_HARMONY_GATE_OUTPUT].pos, "Gate", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_HARMONY_GATE4_OUTPUT].pos, "Gate 4", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_HARMONY_VOLUME_OUTPUT].pos, "Volume", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_MELODY_CV_OUTPUT].pos, "1V/Oct", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_MELODY_GATE_OUTPUT].pos, "Gate", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_MELODY_VOLUME_OUTPUT].pos, "Volume", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_BASS_CV_OUTPUT].pos, "1V/Oct", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_BASS_NOTE2_OUTPUT].pos, "Note 2", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_BASS_GATE_OUTPUT].pos, "Gate", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_BASS_GATE2_OUTPUT].pos, "Gate 2", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_BASS_VOLUME_OUTPUT].pos, "Volume", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_FBM_MELODY_OUTPUT].pos, "Melody", 0, 1);

				nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xff));
				nvgStrokeColor(args.vg,nvgRGBA( 0x80,  0x80 , 0x80, 0x80));
				nvgFontSize(args.vg, 17);
				nvgFontFaceId(args.vg, textfont->handle);
				nvgTextLetterSpacing(args.vg, -1);
				nvgBeginPath(args.vg);
				nvgFontFaceId(args.vg, textfont->handle);
				nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
			    nvgText(args.vg, OutportRectLocal[Meander::OUT_FBM_MELODY_OUTPUT].pos.x+13,  OutportRectLocal[Meander::OUT_FBM_MELODY_OUTPUT].pos.y-30, "Outputs are 0-10V fBm noise", NULL);
				

				drawOutport(args, OutportRectLocal[Meander::OUT_FBM_HARMONY_OUTPUT].pos, "Harmony", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_FBM_ARP_OUTPUT].pos, "32nds", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_CLOCK_BAR_OUTPUT].pos, "Bar", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_CLOCK_BEAT_OUTPUT].pos, "Beat", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_CLOCK_BEATX2_OUTPUT].pos, "Beat  x2", 0, 1);

				rack::math::Rect rect=OutportRectLocal[Meander::OUT_CLOCK_BEATX2_OUTPUT];
				rect.pos=rect.pos.plus(Vec(0,-16));
				drawLabelAbove(args, rect, "1ms Clocked Trigger Pulses", 18.);
				
				drawOutport(args, OutportRectLocal[Meander::OUT_CLOCK_BEATX4_OUTPUT].pos, "Beat x4", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_CLOCK_BEATX8_OUTPUT].pos, "Beat x8", 0, 1);

				rect=OutportRectLocal[Meander::OUT_CLOCK_BEATX8_OUTPUT];
				rect.pos=rect.pos.plus(Vec(5,0));
				drawLabelRight(args, rect, "These can trigger STEP above");
				
				// drawOutport(args, OutportRectLocal[Meander::OUT_EXT_POLY_SCALE_OUTPUT].pos, "Out", 0, 1);

				drawOutport(args, OutportRectLocal[Meander::OUT_CLOCK_OUT].pos, "Out", 0, 1);
			}

			Vec pos;
			char text[128];
			nvgFontSize(args.vg, 17);
			nvgFontFaceId(args.vg, textfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF));
			
			//************************
			// circle area 
			
			float beginEdge = 295;
			float beginTop =115;
			float lineWidth=1.0; 
			float stafflineLength=100;
			float barLineVoffset=36.;
			float barLineVlength=60.;
			int yLineSpacing=6;
			float yHalfLineSpacing=3.0f;

		    // draw bar left vertical edge
			if (beginEdge > 0) {
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, beginEdge, beginTop+barLineVoffset);
				nvgLineTo(args.vg, beginEdge, beginTop+barLineVlength);
				nvgStrokeColor(args.vg, nvgRGB(0, 0, 0));
				nvgStrokeWidth(args.vg, lineWidth);
				nvgStroke(args.vg);
			}
			// draw staff lines
			nvgBeginPath(args.vg);
			for (int staff = 36, y = staff; y <= staff + 24; y += yLineSpacing) { 	
				nvgMoveTo(args.vg, beginEdge, beginTop+y);
				nvgLineTo(args.vg, beginEdge+stafflineLength, beginTop+y);
			}
			nvgStrokeColor(args.vg, nvgRGB(0x7f, 0x7f, 0x7f));
			nvgStrokeWidth(args.vg, lineWidth);
			nvgStroke(args.vg);

			nvgFontSize(args.vg, 45);
			nvgFontFaceId(args.vg, musicfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF));
		//	pos=Vec(beginEdge+10, beginTop+45);  // had to change for some unknown reason
			pos=Vec(beginEdge, beginTop+45);  
			nvgText(args.vg, pos.x, pos.y, "G", NULL);
			
			nvgFontSize(args.vg, 35);
			nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
			
			int num_sharps1=0;
			int vertical_offset1=0;
			for (int i=0; i<7; ++i)
			{
				nvgBeginPath(args.vg);
				if (root_key_signatures_chromaticForder[module->notate_mode_as_signature_root_key][i]==1)
				{
					vertical_offset1=root_key_sharps_vertical_display_offset[num_sharps1];
					pos=Vec(beginEdge+20+(num_sharps1*5), beginTop+24+(vertical_offset1*yHalfLineSpacing));
					nvgText(args.vg, pos.x, pos.y, "B", NULL);
					++num_sharps1;
				}
				nvgClosePath(args.vg);
			}	
		
			int num_flats1=0;
			vertical_offset1=0;
			for (int i=6; i>=0; --i)
			{
				nvgBeginPath(args.vg);
				if (root_key_signatures_chromaticForder[module->notate_mode_as_signature_root_key][i]==-1)
				{
					vertical_offset1=root_key_flats_vertical_display_offset[num_flats1];
					pos=Vec(beginEdge+20+(num_flats1*5), beginTop+24+(vertical_offset1*yHalfLineSpacing));
					nvgText(args.vg, pos.x, pos.y, "b", NULL);
					++num_flats1;
				}
				nvgClosePath(args.vg);
			}	

			nvgFontSize(args.vg, 12);
			nvgFontFaceId(args.vg, textfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF));

			pos=Vec(beginEdge+30, beginTop+95);  
			nvgText(args.vg, pos.x, pos.y, "In", NULL);

			pos=Vec(beginEdge+30, beginTop+115);  
			nvgText(args.vg, pos.x, pos.y, "In", NULL);

			pos=Vec(beginEdge+80, beginTop+95);   
			nvgText(args.vg, pos.x, pos.y, "Degree (1-7)", NULL);

			pos=Vec(beginEdge+74, beginTop+115);  
			nvgText(args.vg, pos.x, pos.y, "Gate", NULL);

			//************************

			// display area 

			// draw staff lines

			beginEdge = 890;
			beginTop =8;
			lineWidth=1.0; 
			
			stafflineLength=300;
			barLineVoffset=36.;
			barLineVlength=60.;
			yLineSpacing=6;
			yHalfLineSpacing=3.0f;

		    // draw bar left vertical edge

			if (beginEdge > 0) {
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, beginEdge, beginTop+barLineVoffset);
				nvgLineTo(args.vg, beginEdge, beginTop+(1.60*barLineVlength));
				nvgStrokeColor(args.vg, nvgRGB(0, 0, 0));
				nvgStrokeWidth(args.vg, lineWidth);
				nvgStroke(args.vg);
			}

			 // draw bar right vertical edge
			if (beginEdge > 0) {
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, beginEdge+stafflineLength, beginTop+barLineVoffset);
				nvgLineTo(args.vg, beginEdge+stafflineLength, beginTop+(1.60*barLineVlength));
				nvgStrokeColor(args.vg, nvgRGB(0, 0, 0));
				nvgStrokeWidth(args.vg, lineWidth);
				nvgStroke(args.vg);
			}
			// draw staff lines
			nvgBeginPath(args.vg);
		
			for (int staff = 36; staff <= 72; staff += 36) {
				for (int y = staff; y <= staff + 24; y += 6) { 
					nvgMoveTo(args.vg, beginEdge, beginTop+y);
					nvgLineTo(args.vg, beginEdge+stafflineLength, beginTop+y);
				}
			}

			nvgStrokeColor(args.vg, nvgRGB(0x7f, 0x7f, 0x7f));
			nvgStrokeWidth(args.vg, lineWidth);
			nvgStroke(args.vg);

			nvgFontSize(args.vg, 45);
			nvgFontFaceId(args.vg, musicfont->handle);
			nvgTextLetterSpacing(args.vg, -1);
			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF));
			pos=Vec(beginEdge+10, beginTop+45);  
			nvgText(args.vg, pos.x, pos.y, "G", NULL);

			nvgFontSize(args.vg, 36);
			pos=Vec(beginEdge+10, beginTop+80); 
			nvgText(args.vg, pos.x, pos.y, "?", NULL);
			
			nvgFontSize(args.vg, 40);
			pos=Vec(beginEdge+53, beginTop+33);
			snprintf(text, sizeof(text), "%d",module->time_sig_top);
			nvgText(args.vg, pos.x, pos.y, text, NULL);

			nvgFontSize(args.vg, 40);
			pos=Vec(beginEdge+53, beginTop+69);
			snprintf(text, sizeof(text), "%d",module->time_sig_top);  
			nvgText(args.vg, pos.x, pos.y, text, NULL);

			nvgFontSize(args.vg, 40);
			pos=Vec(beginEdge+53, beginTop+45);
			snprintf(text, sizeof(text), "%d",module->time_sig_bottom);
			nvgText(args.vg, pos.x, pos.y, text, NULL);

			nvgFontSize(args.vg, 40);
			pos=Vec(beginEdge+53, beginTop+81);
			snprintf(text, sizeof(text), "%d",module->time_sig_bottom);  
			nvgText(args.vg, pos.x, pos.y, text, NULL);

			// do root_key signature
			
			nvgFontSize(args.vg, 35);
			nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
			
			num_sharps1=0;
			vertical_offset1=0;
			for (int i=0; i<7; ++i)
			{
				nvgBeginPath(args.vg);
				if (root_key_signatures_chromaticForder[module->notate_mode_as_signature_root_key][i]==1)
				{
					vertical_offset1=root_key_sharps_vertical_display_offset[num_sharps1];
					pos=Vec(beginEdge+20+(num_sharps1*5), beginTop+24+(vertical_offset1*yHalfLineSpacing));
					nvgText(args.vg, pos.x, pos.y, "B", NULL);
					++num_sharps1;
				}
				nvgClosePath(args.vg);
			}	
		
			num_flats1=0;
			vertical_offset1=0;
			for (int i=6; i>=0; --i)  
			{
				nvgBeginPath(args.vg);
				if (root_key_signatures_chromaticForder[module->notate_mode_as_signature_root_key][i]==-1)
				{
					vertical_offset1=root_key_flats_vertical_display_offset[num_flats1];
					pos=Vec(beginEdge+20+(num_flats1*5), beginTop+24+(vertical_offset1*yHalfLineSpacing));
					nvgText(args.vg, pos.x, pos.y, "b", NULL);
					++num_flats1;
				}
				nvgClosePath(args.vg);
			}	

			// now do for bass clef

			nvgFontSize(args.vg, 35);
			nvgTextAlign(args.vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
			
			num_sharps1=0;
			vertical_offset1=0;
			for (int i=0; i<7; ++i)
			{
				nvgBeginPath(args.vg);
				if (root_key_signatures_chromaticForder[module->notate_mode_as_signature_root_key][i]==1)
				{
					vertical_offset1=root_key_sharps_vertical_display_offset[num_sharps1];
					pos=Vec(beginEdge+20+(num_sharps1*5), beginTop+67+(vertical_offset1*yHalfLineSpacing));
					nvgText(args.vg, pos.x, pos.y, "B", NULL);
					++num_sharps1;
				}
				nvgClosePath(args.vg);
			}	
		
			num_flats1=0;
			vertical_offset1=0;
			for (int i=6; i>=0; --i)
			{
				nvgBeginPath(args.vg);
				if (root_key_signatures_chromaticForder[module->notate_mode_as_signature_root_key][i]==-1)
				{
					vertical_offset1=root_key_flats_vertical_display_offset[num_flats1];
					pos=Vec(beginEdge+20+(num_flats1*5), beginTop+67+(vertical_offset1*yHalfLineSpacing));
					nvgText(args.vg, pos.x, pos.y, "b", NULL);
					++num_flats1;
				}
				nvgClosePath(args.vg);
			}	
			

			//****************
				
			float display_note_position=0; 

			if (globalsInitialized)  // global fully initialized if Module!=NULL
			{
				nvgFontSize(args.vg, 30);
				nvgFontFaceId(args.vg, musicfont->handle);
				nvgTextLetterSpacing(args.vg, -1);
				nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF));

				for (int i=0; ((i<module->bar_note_count)&&(i<256)); ++i)
				{
					int display_note=module->played_notes_circular_buffer[i].note;
					if (doDebug) DEBUG("display_note=%d %s", display_note, module->note_desig[display_note%12]);
				
					int scale_note=0;
					if (strstr(module->note_desig[display_note%12],"C"))
						scale_note=0;
					else
					if (strstr(module->note_desig[display_note%12],"D"))
						scale_note=1;
					else
					if (strstr(module->note_desig[display_note%12],"E"))
						scale_note=2;
					else
					if (strstr(module->note_desig[display_note%12],"F"))
						scale_note=3;
					else
					if (strstr(module->note_desig[display_note%12],"G"))
						scale_note=4;
					else
					if (strstr(module->note_desig[display_note%12],"A"))
						scale_note=5;
					else
					if (strstr(module->note_desig[display_note%12],"B"))
						scale_note=6;
					if (doDebug) DEBUG("scale_note=%d", scale_note%12);
				
					int octave=(display_note/12)-2;
					if (doDebug) DEBUG("octave=%d", octave);
				
					display_note_position = 108.0-(octave*21.0)-(scale_note*3.0)-7.5;
					if (doDebug) DEBUG("display_note_position=%d", (int)display_note_position);
				
					
					float note_x_spacing= 230.0/(32*module->time_sig_top/module->time_sig_bottom);  // experimenting with note spacing function of time_signature.  barts_count_limit is not in scope, needs to be global
					pos=Vec(beginEdge+70+(module->played_notes_circular_buffer[i].time32s*note_x_spacing), beginTop+display_note_position);  
					if (true)  // color code notes in staff rendering
					{ 
						if (module->played_notes_circular_buffer[i].noteType==NOTE_TYPE_CHORD)
							nvgFillColor(args.vg, nvgRGBA(0xFF, 0x0, 0x0, 0xFF)); 
						else
						if (module->played_notes_circular_buffer[i].noteType==NOTE_TYPE_MELODY)
							nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF)); 
						else
						if (module->played_notes_circular_buffer[i].noteType==NOTE_TYPE_ARP)
							nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0xFF, 0xFF)); 
						else
						if (module->played_notes_circular_buffer[i].noteType==NOTE_TYPE_BASS)
							nvgFillColor(args.vg, nvgRGBA(0x0, 0xFF, 0x0, 0xFF)); 
						
					}

					
					nvgFontSize(args.vg, 30);
					if (module->played_notes_circular_buffer[i].length==1)
						snprintf(text, sizeof(text), "%s", "w");  // mnemonic W=whole, h=half, q-quarter, e=eighth, s=sixteenth notes
					else
					if (module->played_notes_circular_buffer[i].length==2)
						snprintf(text, sizeof(text), "%s", "h");  // mnemonic W=whole, h=half, q-quarter, e=eighth, s=sixteenth notes
					else
					if (module->played_notes_circular_buffer[i].length==4)
						snprintf(text, sizeof(text), "%s", "q");  // mnemonic W=whole, h=half, q-quarter, e=eighth, s=sixteenth notes
					else
					if (module->played_notes_circular_buffer[i].length==8)
						snprintf(text, sizeof(text), "%s", "e");  // mnemonic W=whole, h=half, q-quarter, e=eighth, s=sixteenth notes
					else
					if (module->played_notes_circular_buffer[i].length==16)
						snprintf(text, sizeof(text), "%s", "s");  // mnemonic W=whole, h=half, q-quarter, e=eighth, s=sixteenth notes
					else
					if (module->played_notes_circular_buffer[i].length==32)
						snprintf(text, sizeof(text), "%s", "s");  // mnemonic W=whole, h=half, q-quarter, e=eighth, s=sixteenth notes
					nvgText(args.vg, pos.x, pos.y, text, NULL);

					if (module->played_notes_circular_buffer[i].length==32)  // do overstrike for 1/32 symbol
					{
						nvgFontSize(args.vg, 15);
						nvgText(args.vg, pos.x-.5, pos.y+4.5, "e", NULL);
					
					}


					if (((scale_note==5)&&(octave==3))  //A3
					||((scale_note==0)&&(octave==4))  //C4
					||((scale_note==0)&&(octave==2))  //C2
					||((scale_note==2)&&(octave==0))  //E0 
					||((scale_note==0)&&(octave==0))) //C0 
					{
						nvgFontSize(args.vg, 30);
						pos.x -= 2.0;
						pos.y -= 4.4;
						nvgText(args.vg, pos.x, pos.y, "_", NULL);
					} 
				}
			}
			
			//*********************

			if (globalsInitialized)  // global fully initialized if Module!=NULL
			{
				nvgFontSize(args.vg, 14);
				nvgFontFaceId(args.vg, textfont->handle);
				nvgTextLetterSpacing(args.vg, -1);

				// write last melody note played 
				pos=convertSVGtoNVG(261.4, 121.0, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
				snprintf(text, sizeof(text), "%s%d", module->note_desig[(module->theMeanderState.theMelodyParms.last[0].note%12)], (int)(module->theMeanderState.theMelodyParms.last[0].note/12 ));
				nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF)); 
				nvgText(args.vg, pos.x, pos.y, text, NULL);

				// write last arp note played 
				if (module->theMeanderState.theArpParms.note_count>0)
				{
					pos=convertSVGtoNVG(261.4, 120.3, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
					snprintf(text, sizeof(text), "%s%d", module->note_desig[(module->theMeanderState.theArpParms.last[module->theMeanderState.theArpParms.note_count].note%12)], (int)(module->theMeanderState.theArpParms.last[module->theMeanderState.theArpParms.note_count].note/12 ));
					nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0xFF, 0xFF)); 
					nvgText(args.vg, pos.x+20, pos.y+200, text, NULL);
				}
				
				// write last harmony note played 1
				pos=convertSVGtoNVG(187.8, 121.0, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
				snprintf(text, sizeof(text), "%s%d", module->note_desig[(module->theMeanderState.theHarmonyParms.last[0].note%12)] , module->theMeanderState.theHarmonyParms.last[0].note/12);
				nvgFillColor(args.vg, nvgRGBA(0xFF, 0x0, 0x0, 0xFF)); 
				nvgText(args.vg, pos.x, pos.y, text, NULL);

				// write last harmony note played 2
				pos=convertSVGtoNVG(199.1, 121.0, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
				snprintf(text, sizeof(text), "%s%d", module->note_desig[(module->theMeanderState.theHarmonyParms.last[1].note%12)], module->theMeanderState.theHarmonyParms.last[1].note/12);
				nvgFillColor(args.vg, nvgRGBA(0xFF, 0x0, 0x0, 0xFF)); 
				nvgText(args.vg, pos.x, pos.y, text, NULL);

				// write last harmony note played 3
				pos=convertSVGtoNVG(210.4, 121.0, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
				snprintf(text, sizeof(text), "%s%d", module->note_desig[(module->theMeanderState.theHarmonyParms.last[2].note%12)], module->theMeanderState.theHarmonyParms.last[2].note/12);
				nvgFillColor(args.vg, nvgRGBA(0xFF, 0x0, 0x0, 0xFF)); 
				nvgText(args.vg, pos.x, pos.y, text, NULL);

				// write last harmony note played 4
				pos=convertSVGtoNVG(221.7, 121.0, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
				if ((module->theMeanderState.theHarmonyParms.last_chord_type==2)||(module->theMeanderState.theHarmonyParms.last_chord_type==3)||(module->theMeanderState.theHarmonyParms.last_chord_type==4)||(module->theMeanderState.theHarmonyParms.last_chord_type==5))
					snprintf(text, sizeof(text), "%s%d", module->note_desig[(module->theMeanderState.theHarmonyParms.last[3].note%12)], module->theMeanderState.theHarmonyParms.last[3].note/12);
				else
					snprintf(text, sizeof(text), "%s", "   ");
				nvgFillColor(args.vg, nvgRGBA(0xFF, 0x0, 0x0, 0xFF)); 
				nvgText(args.vg, pos.x, pos.y, text, NULL);
				
						
				// write last bass note played 
				pos=convertSVGtoNVG(319.1, 121.0, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
				snprintf(text, sizeof(text), "%s%d", module->note_desig[(module->theMeanderState.theBassParms.last[0].note%12)], (module->theMeanderState.theBassParms.last[0].note/12));
				nvgFillColor(args.vg, nvgRGBA(0x0, 0xF0, 0x0, 0xF0)); 
				nvgText(args.vg, pos.x, pos.y, text, NULL);

				// write last octave bass note played 
				if (module->theMeanderState.theBassParms.octave_enabled)
				{
					pos=convertSVGtoNVG(330.1, 121.0, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
					snprintf(text, sizeof(text), "%s%d", module->note_desig[(module->theMeanderState.theBassParms.last[1].note%12)], (module->theMeanderState.theBassParms.last[1].note/12));
					nvgFillColor(args.vg, nvgRGBA(0x0, 0xE0, 0x0, 0xFF)); 
					nvgText(args.vg, pos.x, pos.y, text, NULL);
				}
			}

			int last_chord_root=module->theMeanderState.last_harmony_chord_root_note%12;
			int last_chord_bass_note=module->theMeanderState.theHarmonyParms.last[0].note%12;
		//	pos=convertSVGtoNVG(110, 60, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
			pos=convertSVGtoNVG(110, 62, 12.1, 6.5);  // X,Y,W,H in Inkscape mm units
			nvgFontSize(args.vg, 30);

			char chord_type_desc[16];
			if (module->theMeanderState.theHarmonyParms.last_chord_type==0)
				strcpy(chord_type_desc, "");
			if (module->theMeanderState.theHarmonyParms.last_chord_type==2)  // dom
				strcpy(chord_type_desc, "dom7");
			if (module->theMeanderState.theHarmonyParms.last_chord_type==1)
				strcpy(chord_type_desc, "m");
			if (module->theMeanderState.theHarmonyParms.last_chord_type==3)
				strcpy(chord_type_desc, "7");
			if (module->theMeanderState.theHarmonyParms.last_chord_type==4)
				strcpy(chord_type_desc, "m7");
			if (module->theMeanderState.theHarmonyParms.last_chord_type==5)
				strcpy(chord_type_desc, "dim7");
			if (module->theMeanderState.theHarmonyParms.last_chord_type==6)
				strcpy(chord_type_desc, "dim");

			if (last_chord_bass_note!=last_chord_root) 
				snprintf(text, sizeof(text), "%s%s/%s", module->note_desig[last_chord_root], chord_type_desc, module->note_desig[last_chord_bass_note]);
			else
				snprintf(text, sizeof(text), "%s%s", module->note_desig[last_chord_root], chord_type_desc);

			nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF));
			nvgText(args.vg, pos.x, pos.y, text, NULL);
			
		//	drawGrid(args);  // here after all updates are completed so grid is on top


					
			if (doDebug) DEBUG("UpdatePanel()-end");
		}  // end UpdatePanel()

	   
		double smoothedDt=.016;  // start out at 1/60
		int numZeroes=0;
		
		void draw(const DrawArgs &args) override 
		{   
			if (true)  // disable nanovg rendering for testing
			{
				#if defined(_USE_WINDOWS_PERFTIME)
				LARGE_INTEGER t;
				QueryPerformanceFrequency(&t);
				double frequency=double(t.QuadPart);
				QueryPerformanceCounter(&t);
				int64_t n= t.QuadPart;
				double count= (double)(n)/frequency;
				double time1=count;
				#endif

									

				DrawCircle5ths(args, module->root_key);  // has to be done each frame as panel redraws as SVG and needs to be blanked and cirecles redrawn
				DrawDegreesSemicircle(args,  module->root_key);
				updatePanel(args);
				 
				
				#if defined(_USE_WINDOWS_PERFTIME)
				QueryPerformanceCounter(&t);
				n= t.QuadPart;
				count= (double)(n)/frequency;
				double time2=count;
				double deltaTime=time2-time1;
				smoothedDt=(.9*smoothedDt) + (.1*(deltaTime));
				
				nvgFontSize(args.vg, 12);
				nvgFontFaceId(args.vg, textfont->handle);
				nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE); 
				nvgTextLetterSpacing(args.vg, -1);
				nvgFillColor(args.vg, nvgRGBA(0x0, 0x0, 0x0, 0xFF));
				Vec pos=Vec(10, 338);
				char text[128];
				snprintf(text, sizeof(text), "UI   %.2lf ms", smoothedDt*1000.0);
				nvgText(args.vg, pos.x, pos.y, text, NULL);
				#endif
				
			} 
		}

		
	
	};  // end struct CircleOf5thsDisplay

	CircleOf5thsDisplay *display;

	MeanderWidget(Meander* module)   // all plugins I've looked at use this constructor with module*, even though docs show it deprecated.  
	{ 
		if (doDebug) DEBUG("MeanderWidget()");
		setModule(module);  // most plugins do this
		this->module = module;  //  most plugins do not do this.  It was introduced in singleton implementation

		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Meander.svg")));
					
		// rack::random::init();  // must be called per thread

			
		 if (true)   // must be executed in order to see ModuleWidget panel display in preview, module* is checked for null below as it is null in browser preview
		 {
			display = new CircleOf5thsDisplay();
			display->module = module;
			display->canSquash = true;
			display->ParameterRectLocal=ParameterRect;
			display->InportRectLocal=InportRect;  
			display->OutportRectLocal=OutportRect;  
									
			display->box.pos = Vec(0, 0);
			display->box.size = Vec(box.size.x, box.size.y);
			addChild(display);

			RootKeySelectLineDisplay *MeanderRootKeySelectDisplay = createWidget<RootKeySelectLineDisplay>(Vec(120.,198.));  
			MeanderRootKeySelectDisplay->module = module;
			MeanderRootKeySelectDisplay->box.size = Vec(40, 22); 
			addChild(MeanderRootKeySelectDisplay);

			ScaleSelectLineDisplay *MeanderScaleSelectDisplay = createWidget<ScaleSelectLineDisplay>(Vec(40.,225.));  
			MeanderScaleSelectDisplay->module = module;
			MeanderScaleSelectDisplay->box.size = Vec(120, 22); 
			addChild(MeanderScaleSelectDisplay);			
			
			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

			//BPM DISPLAY  
			BpmDisplayWidget *BPMdisplay = new BpmDisplayWidget();
			BPMdisplay->box.pos = Vec(70,90);
			BPMdisplay->box.size = Vec(80, 35);
			if (module) 
				BPMdisplay->val = &module->tempo;
			addChild(BPMdisplay); 
			
			//SIG TOP DISPLAY 
			SigDisplayWidget *SigTopDisplay = new SigDisplayWidget();
			SigTopDisplay->box.pos = Vec(130,130);
			SigTopDisplay->box.size = Vec(25, 20);
			if (module) 
				SigTopDisplay->value = &module->time_sig_top;
			addChild(SigTopDisplay);
			//SIG TOP KNOB
		
			//SIG BOTTOM DISPLAY    
			SigDisplayWidget *SigBottomDisplay = new SigDisplayWidget();
			SigBottomDisplay->box.pos = Vec(130,150);
			SigBottomDisplay->box.size = Vec(25, 20);
			if (module) 
				SigBottomDisplay->value = &module->time_sig_bottom;
			addChild(SigBottomDisplay);
			
			//*************   Note: Each LEDButton needs its light and that light needs a unique ID, needs to be added to an array and then needs to be repositioned along with the button.  Also needs to be enumed with other lights so lights[] picks it up.
			paramWidgets[Meander::BUTTON_CIRCLESTEP_C_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(116.227, 37.257)), module, Meander::BUTTON_CIRCLESTEP_C_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_C_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_1]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(116.227, 37.257)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_1);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_1]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_G_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(132.479, 41.32)), module, Meander::BUTTON_CIRCLESTEP_G_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_G_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_2]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(132.479, 41.32)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_2);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_2]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_D_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(143.163, 52.155)), module, Meander::BUTTON_CIRCLESTEP_D_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_D_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_3]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(143.163, 52.155)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_3);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_3]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_A_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(147.527, 67.353)), module, Meander::BUTTON_CIRCLESTEP_A_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_A_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_4]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(147.527, 67.353)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_4);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_4]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_E_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(141.96, 83.906)), module, Meander::BUTTON_CIRCLESTEP_E_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_E_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_5]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(141.96, 83.906)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_5);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_5]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_B_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(132.931, 94.44)), module, Meander::BUTTON_CIRCLESTEP_B_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_B_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_6]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(132.931, 94.44)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_6);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_6]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_GBFS_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(116.378, 98.804)), module, Meander::BUTTON_CIRCLESTEP_GBFS_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_GBFS_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_7]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(116.378, 98.804)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_7);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_7]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_DB_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(101.029, 93.988)), module, Meander::BUTTON_CIRCLESTEP_DB_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_DB_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_8]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(101.029, 93.988)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_8);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_8]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_AB_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(91.097, 83.906)), module, Meander::BUTTON_CIRCLESTEP_AB_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_AB_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_9]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(91.097, 83.906)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_9);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_9]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_EB_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(86.282, 68.106)), module, Meander::BUTTON_CIRCLESTEP_EB_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_EB_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_10]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(86.282, 68.106)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_10);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_10]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_BB_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(89.743, 52.004)), module, Meander::BUTTON_CIRCLESTEP_BB_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_BB_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_11]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(189.743, 52.004)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_11);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_11]);
		
			paramWidgets[Meander::BUTTON_CIRCLESTEP_F_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(101.781, 40.568)), module, Meander::BUTTON_CIRCLESTEP_F_PARAM);
			addParam(paramWidgets[Meander::BUTTON_CIRCLESTEP_F_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_12]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(101.781, 40.568)), module, Meander::LIGHT_LEDBUTTON_CIRCLESTEP_12);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_12]);
		
	//*************
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(116.227, 43.878)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_2_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(129.018, 47.189)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_2_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_2_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_3_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(137.295, 56.067)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_3_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_3_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_4_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(140.455, 67.654)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_4_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_4_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_5_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(137.144, 80.295)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_5_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_5_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_6_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(128.868, 88.571)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_6_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_6_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_7_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(116.077, 92.183)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_7_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_7_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_8_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(104.791, 88.872)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_8_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_8_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_9_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(96.213, 80.596)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_9_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_9_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_10_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(92.602, 67.654)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_10_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_10_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_11_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(95.912, 55.465)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_11_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_11_LIGHT]);
		
			lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_12_LIGHT]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(105.393, 46.587)), module, Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_12_LIGHT);
			addChild(lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_12_LIGHT]);
				 		

	//*********** Harmony step set selection    

					
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_1_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(65.197, 106.483)), module, Meander::BUTTON_HARMONY_SETSTEP_1_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_1_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_1]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(65.197, 106.483)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_1);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_1]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_2_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.918, 98.02)), module, Meander::BUTTON_HARMONY_SETSTEP_2_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_2_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_2]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.918, 98.02)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_2);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_2]);

		
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_3_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(65.193, 89.271)), module, Meander::BUTTON_HARMONY_SETSTEP_3_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_3_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_3]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(65.193, 89.271)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_3);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_3]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_4_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.918, 81.9233)), module, Meander::BUTTON_HARMONY_SETSTEP_4_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_4_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_4]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.918, 81.923)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_4);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_4]);

		
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_5_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(65.193, 73.184)), module, Meander::BUTTON_HARMONY_SETSTEP_5_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_5_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_5]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(65.193, 73.184)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_5);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_5]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_6_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.918, 66.129)), module, Meander::BUTTON_HARMONY_SETSTEP_6_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_6_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_6]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.918, 66.129)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_6);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_6]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_7_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(65.193, 57.944)), module, Meander::BUTTON_HARMONY_SETSTEP_7_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_7_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_7]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(65.193, 57.944)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_7);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_7]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_8_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.911, 49.474)), module, Meander::BUTTON_HARMONY_SETSTEP_8_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_8_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_8]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.911, 49.474)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_8);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_8]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_9_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(4.629, 41.011)), module, Meander::BUTTON_HARMONY_SETSTEP_9_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_9_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_9]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(4.629, 41.011)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_9);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_9]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_10_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.629, 32.827)), module, Meander::BUTTON_HARMONY_SETSTEP_10_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_10_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_10]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.629, 32.827)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_10);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_10]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_11_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.629, 24.649)), module, Meander::BUTTON_HARMONY_SETSTEP_11_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_11_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_11]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.629, 24.649)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_11);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_11]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_12_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.632, 16.176)), module, Meander::BUTTON_HARMONY_SETSTEP_12_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_12_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_12]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.632, 16.176)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_12);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_12]);

		
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_13_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.632, 16.176)), module, Meander::BUTTON_HARMONY_SETSTEP_13_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_13_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_13]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.632, 16.176)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_13);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_13]);

		
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_14_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.632, 16.176)), module, Meander::BUTTON_HARMONY_SETSTEP_14_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_14_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_14]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.632, 16.176)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_14);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_14]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_15_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.632, 16.176)), module, Meander::BUTTON_HARMONY_SETSTEP_15_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_15_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_15]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.632, 16.176)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_15);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_15]);

			
			paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_16_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(64.632, 16.176)), module, Meander::BUTTON_HARMONY_SETSTEP_16_PARAM);
			addParam(paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_16_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_16]=createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(64.632, 16.176)), module, Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_16);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_16]);

			//**********General************************
			
			paramWidgets[Meander::BUTTON_RUN_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(19.7, 10.45)), module, Meander::BUTTON_RUN_PARAM);
			addParam(paramWidgets[Meander::BUTTON_RUN_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_RUN]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.7, 10.45)), module, Meander::LIGHT_LEDBUTTON_RUN);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_RUN]);
        
			paramWidgets[Meander::BUTTON_RESET_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(19.7, 22.55)), module, Meander::BUTTON_RESET_PARAM);
			addParam(paramWidgets[Meander::BUTTON_RESET_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_RESET]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.7, 22.55)), module, Meander::LIGHT_LEDBUTTON_RESET);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_RESET]);
         
			paramWidgets[Meander::CONTROL_TEMPOBPM_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(8.12, 35.4)), module, Meander::CONTROL_TEMPOBPM_PARAM);
			addParam(paramWidgets[Meander::CONTROL_TEMPOBPM_PARAM]);
			
			paramWidgets[Meander::CONTROL_TIMESIGNATURETOP_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(8.12, 47.322+3.0)), module, Meander::CONTROL_TIMESIGNATURETOP_PARAM);
			addParam(paramWidgets[Meander::CONTROL_TIMESIGNATURETOP_PARAM]);
		
			paramWidgets[Meander::CONTROL_TIMESIGNATUREBOTTOM_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(8.12, 54.84+3.0)), module, Meander::CONTROL_TIMESIGNATUREBOTTOM_PARAM);
			addParam(paramWidgets[Meander::CONTROL_TIMESIGNATUREBOTTOM_PARAM]);
			
			paramWidgets[Meander::CONTROL_ROOT_KEY_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(8.12, 68.179)), module, Meander::CONTROL_ROOT_KEY_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_ROOT_KEY_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_ROOT_KEY_PARAM]);
			
			paramWidgets[Meander::CONTROL_SCALE_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(8.12, 78.675)), module, Meander::CONTROL_SCALE_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_SCALE_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_SCALE_PARAM]);

			//***************Harmony******************
						
			paramWidgets[Meander::BUTTON_ENABLE_HARMONY_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(173.849, 12.622)), module, Meander::BUTTON_ENABLE_HARMONY_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_HARMONY_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_HARMONY_ENABLE]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(173.849, 12.622)), module, Meander::LIGHT_LEDBUTTON_HARMONY_ENABLE);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_HARMONY_ENABLE]);
		
			paramWidgets[Meander::CONTROL_HARMONY_VOLUME_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(173.849, 20.384)), module, Meander::CONTROL_HARMONY_VOLUME_PARAM);
			addParam(paramWidgets[Meander::CONTROL_HARMONY_VOLUME_PARAM]);
				
			paramWidgets[Meander::CONTROL_HARMONY_STEPS_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(174.028, 28)), module, Meander::CONTROL_HARMONY_STEPS_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_HARMONY_STEPS_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_HARMONY_STEPS_PARAM]);
		
			paramWidgets[Meander::CONTROL_HARMONY_TARGETOCTAVE_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(174.01, 37.396)), module, Meander::CONTROL_HARMONY_TARGETOCTAVE_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_HARMONY_TARGETOCTAVE_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_HARMONY_TARGETOCTAVE_PARAM]);
			
			paramWidgets[Meander::CONTROL_HARMONY_ALPHA_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(174.27, 45.982)), module, Meander::CONTROL_HARMONY_ALPHA_PARAM);
			addParam(paramWidgets[Meander::CONTROL_HARMONY_ALPHA_PARAM]);

			paramWidgets[Meander::CONTROL_HARMONY_RANGE_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(173.991, 53.788)), module, Meander::CONTROL_HARMONY_RANGE_PARAM);
			addParam(paramWidgets[Meander::CONTROL_HARMONY_RANGE_PARAM]);
			  
			paramWidgets[Meander::CONTROL_HARMONY_DIVISOR_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(173.953, 62.114)), module, Meander::CONTROL_HARMONY_DIVISOR_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_HARMONY_DIVISOR_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_HARMONY_DIVISOR_PARAM]);
			
			paramWidgets[Meander::BUTTON_ENABLE_HARMONY_ALL7THS_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(173.849, 69)), module, Meander::BUTTON_ENABLE_HARMONY_ALL7THS_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_HARMONY_ALL7THS_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_ALL7THS_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(173.849, 69)), module, Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_ALL7THS_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_ALL7THS_PARAM]);
			
			paramWidgets[Meander::BUTTON_ENABLE_HARMONY_V7THS_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(203.849, 69)), module, Meander::BUTTON_ENABLE_HARMONY_V7THS_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_HARMONY_V7THS_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_V7THS_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(203.849, 69)), module, Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_V7THS_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_V7THS_PARAM]);
			
			paramWidgets[Meander::BUTTON_ENABLE_HARMONY_STACCATO_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(173.849, 75)), module, Meander::BUTTON_ENABLE_HARMONY_STACCATO_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_HARMONY_STACCATO_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_STACCATO_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(173.849, 75)), module, Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_STACCATO_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_STACCATO_PARAM]);
			
			paramWidgets[Meander::CONTROL_HARMONYPRESETS_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(174.027, 81.524)), module, Meander::CONTROL_HARMONYPRESETS_PARAM);
	 		dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_HARMONYPRESETS_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_HARMONYPRESETS_PARAM]);

			//**************Melody********************
						
			paramWidgets[Meander::BUTTON_ENABLE_MELODY_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(240.353, 10.986)), module, Meander::BUTTON_ENABLE_MELODY_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_MELODY_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(240.353, 10.986)), module, Meander::LIGHT_LEDBUTTON_MELODY_ENABLE);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE]);
			
			paramWidgets[Meander::BUTTON_ENABLE_MELODY_CHORDAL_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(270.353, 10.986)), module, Meander::BUTTON_ENABLE_MELODY_CHORDAL_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_MELODY_CHORDAL_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_CHORDAL]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(270.353, 10.986)), module, Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_CHORDAL);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_CHORDAL]);
			
			paramWidgets[Meander::BUTTON_ENABLE_MELODY_SCALER_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(287.274, 10.986)), module, Meander::BUTTON_ENABLE_MELODY_SCALER_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_MELODY_SCALER_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_SCALER]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(287.274, 10.986)), module, Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_SCALER);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_SCALER]);
			
			paramWidgets[Meander::BUTTON_MELODY_DESTUTTER_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(240.409, 25.524)), module, Meander::BUTTON_MELODY_DESTUTTER_PARAM);
			addParam(paramWidgets[Meander::BUTTON_MELODY_DESTUTTER_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_DESTUTTER]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(240.409, 25.524)), module, Meander::LIGHT_LEDBUTTON_MELODY_DESTUTTER);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_DESTUTTER]);
			
			paramWidgets[Meander::CONTROL_MELODY_VOLUME_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.353, 19.217)), module, Meander::CONTROL_MELODY_VOLUME_PARAM);
			addParam(paramWidgets[Meander::CONTROL_MELODY_VOLUME_PARAM]);
 		
			paramWidgets[Meander::CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.353, 32.217)), module, Meander::CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM]);
			
			paramWidgets[Meander::CONTROL_MELODY_TARGETOCTAVE_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.353, 39.217)), module, Meander::CONTROL_MELODY_TARGETOCTAVE_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_MELODY_TARGETOCTAVE_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_MELODY_TARGETOCTAVE_PARAM]);
			
			paramWidgets[Meander::CONTROL_MELODY_ALPHA_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.334, 47.803)), module, Meander::CONTROL_MELODY_ALPHA_PARAM);
			addParam(paramWidgets[Meander::CONTROL_MELODY_ALPHA_PARAM]);

			paramWidgets[Meander::CONTROL_MELODY_RANGE_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.334, 55.33)), module, Meander::CONTROL_MELODY_RANGE_PARAM);
			addParam(paramWidgets[Meander::CONTROL_MELODY_RANGE_PARAM]);

			//*******************Arp********************
			
			paramWidgets[Meander::BUTTON_ENABLE_ARP_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(240.274, 62.01)), module, Meander::BUTTON_ENABLE_ARP_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_ARP_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(240.274, 62.01)), module, Meander::LIGHT_LEDBUTTON_ARP_ENABLE);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE]);

			paramWidgets[Meander::BUTTON_ENABLE_ARP_CHORDAL_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(265.274, 62.01)), module, Meander::BUTTON_ENABLE_ARP_CHORDAL_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_ARP_CHORDAL_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE_CHORDAL]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(265.274, 62.01)), module, Meander::LIGHT_LEDBUTTON_ARP_ENABLE_CHORDAL);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE_CHORDAL]);

			paramWidgets[Meander::BUTTON_ENABLE_ARP_SCALER_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(283.274, 62.01)), module, Meander::BUTTON_ENABLE_ARP_SCALER_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_ARP_SCALER_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE_SCALER]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(283.274, 62.01)), module, Meander::LIGHT_LEDBUTTON_ARP_ENABLE_SCALER);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE_SCALER]);

			paramWidgets[Meander::CONTROL_ARP_COUNT_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.274, 68.014)), module, Meander::CONTROL_ARP_COUNT_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_ARP_COUNT_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_ARP_COUNT_PARAM]);
			
			paramWidgets[Meander::BUTTON_ENABLE_MELODY_STACCATO_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(240.274, 75)), module, Meander::BUTTON_ENABLE_MELODY_STACCATO_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_MELODY_STACCATO_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_MELODY_STACCATO_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(240.274, 75)), module, Meander::LIGHT_LEDBUTTON_ENABLE_MELODY_STACCATO_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_MELODY_STACCATO_PARAM]);

			paramWidgets[Meander::CONTROL_ARP_INCREMENT_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.256, 82.807)), module, Meander::CONTROL_ARP_INCREMENT_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_ARP_INCREMENT_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_ARP_INCREMENT_PARAM]);
		
			paramWidgets[Meander::CONTROL_ARP_DECAY_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.237, 91.691)), module, Meander::CONTROL_ARP_DECAY_PARAM);
			addParam(paramWidgets[Meander::CONTROL_ARP_DECAY_PARAM]);
		
			paramWidgets[Meander::CONTROL_ARP_PATTERN_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(240.757, 99.497)), module, Meander::CONTROL_ARP_PATTERN_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_ARP_PATTERN_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_ARP_PATTERN_PARAM]);

			//*************Bass********************
		
			paramWidgets[Meander::BUTTON_ENABLE_BASS_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(305, 10.378)), module, Meander::BUTTON_ENABLE_BASS_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_BASS_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_ENABLE]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(305, 10.378)), module, Meander::LIGHT_LEDBUTTON_BASS_ENABLE);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_ENABLE]);
			
			paramWidgets[Meander::CONTROL_BASS_VOLUME_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(305, 21.217)), module, Meander::CONTROL_BASS_VOLUME_PARAM);
			addParam(paramWidgets[Meander::CONTROL_BASS_VOLUME_PARAM]);
		
			paramWidgets[Meander::CONTROL_BASS_TARGETOCTAVE_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(305,  29.217)), module, Meander::CONTROL_BASS_TARGETOCTAVE_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_BASS_TARGETOCTAVE_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_BASS_TARGETOCTAVE_PARAM]);
			
			paramWidgets[Meander::BUTTON_BASS_ACCENT_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(305,  37.217)), module, Meander::BUTTON_BASS_ACCENT_PARAM);
			addParam(paramWidgets[Meander::BUTTON_BASS_ACCENT_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_ACCENT_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(305,  37.217)), module, Meander::LIGHT_LEDBUTTON_BASS_ACCENT_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_ACCENT_PARAM]);
			
			paramWidgets[Meander::BUTTON_BASS_SYNCOPATE_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(305,  45.217)), module, Meander::BUTTON_BASS_SYNCOPATE_PARAM);
			addParam(paramWidgets[Meander::BUTTON_BASS_SYNCOPATE_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_SYNCOPATE_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(305,  45.217)), module, Meander::LIGHT_LEDBUTTON_BASS_SYNCOPATE_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_SYNCOPATE_PARAM]);
			
			paramWidgets[Meander::BUTTON_BASS_SHUFFLE_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(305,  53.217)), module, Meander::BUTTON_BASS_SHUFFLE_PARAM);
			addParam(paramWidgets[Meander::BUTTON_BASS_SHUFFLE_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_SHUFFLE_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(305,  53.217)), module, Meander::LIGHT_LEDBUTTON_BASS_SHUFFLE_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_SHUFFLE_PARAM]);

			paramWidgets[Meander::BUTTON_BASS_OCTAVES_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(305,  53.217)), module, Meander::BUTTON_BASS_OCTAVES_PARAM);
			addParam(paramWidgets[Meander::BUTTON_BASS_OCTAVES_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_OCTAVES_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(305,  53.217)), module, Meander::LIGHT_LEDBUTTON_BASS_OCTAVES_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_OCTAVES_PARAM]);
			
			paramWidgets[Meander::CONTROL_BASS_DIVISOR_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(305, 61.217)), module, Meander::CONTROL_BASS_DIVISOR_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_BASS_DIVISOR_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_BASS_DIVISOR_PARAM]);
			
			paramWidgets[Meander::BUTTON_ENABLE_BASS_STACCATO_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(305, 70)), module, Meander::BUTTON_ENABLE_BASS_STACCATO_PARAM);
			addParam(paramWidgets[Meander::BUTTON_ENABLE_BASS_STACCATO_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_BASS_STACCATO_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(305, 70)), module, Meander::LIGHT_LEDBUTTON_ENABLE_BASS_STACCATO_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_BASS_STACCATO_PARAM]);

			//**************fBm************************
						
			paramWidgets[Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(358, 19)), module, Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM]);
			
			paramWidgets[Meander::CONTROL_HARMONY_FBM_PERIOD_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(358, 25)), module, Meander::CONTROL_HARMONY_FBM_PERIOD_PARAM);
			addParam(paramWidgets[Meander::CONTROL_HARMONY_FBM_PERIOD_PARAM]);
		
			paramWidgets[Meander::CONTROL_MELODY_FBM_OCTAVES_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(358, 34)), module, Meander::CONTROL_MELODY_FBM_OCTAVES_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_MELODY_FBM_OCTAVES_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_MELODY_FBM_OCTAVES_PARAM]);
			
			paramWidgets[Meander::CONTROL_MELODY_FBM_PERIOD_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(358, 43)), module, Meander::CONTROL_MELODY_FBM_PERIOD_PARAM);
			addParam(paramWidgets[Meander::CONTROL_MELODY_FBM_PERIOD_PARAM]);
		
			paramWidgets[Meander::CONTROL_ARP_FBM_OCTAVES_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(358, 51)), module, Meander::CONTROL_ARP_FBM_OCTAVES_PARAM);
			dynamic_cast<Knob*>(paramWidgets[Meander::CONTROL_ARP_FBM_OCTAVES_PARAM])->snap=true;
			addParam(paramWidgets[Meander::CONTROL_ARP_FBM_OCTAVES_PARAM]);
			
			paramWidgets[Meander::CONTROL_ARP_FBM_PERIOD_PARAM]=createParamCentered<Trimpot>(mm2px(Vec(358, 59)), module, Meander::CONTROL_ARP_FBM_PERIOD_PARAM);
			addParam(paramWidgets[Meander::CONTROL_ARP_FBM_PERIOD_PARAM]);

			// Progression control

			paramWidgets[Meander::BUTTON_PROG_STEP_PARAM]=createParamCentered<LEDButton>(mm2px(Vec(350, 250)), module, Meander::BUTTON_PROG_STEP_PARAM);
			addParam(paramWidgets[Meander::BUTTON_PROG_STEP_PARAM]);
			lightWidgets[Meander::LIGHT_LEDBUTTON_PROG_STEP_PARAM]=createLightCentered<MediumLight<RedLight>>(mm2px(Vec(350, 250)), module, Meander::LIGHT_LEDBUTTON_PROG_STEP_PARAM);
			addChild(lightWidgets[Meander::LIGHT_LEDBUTTON_PROG_STEP_PARAM]);
			
					 
	//**************  

	// add input ports
		
			for (int i=0; i<Meander::NUM_INPUTS; ++i)
			{
				if (i==Meander::IN_HARMONY_DESTUTTER_EXT_CV)  // this inport is not used, so set to null
					inPortWidgets[i]=NULL;
				else
				{
					inPortWidgets[i]=createInputCentered<TinyPJ301MPort>(mm2px(Vec(10*i,5)), module, i);  // temporarily place them along the top before they are repositioned above
					addInput(inPortWidgets[i]);
				}
			}

	// add output ports		
			
			outPortWidgets[Meander::OUT_RUN_OUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(30.2, 10.95)), module, Meander::OUT_RUN_OUT);
			addOutput(outPortWidgets[Meander::OUT_RUN_OUT]);

			outPortWidgets[Meander::OUT_RESET_OUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(30.2, 23.35)), module, Meander::OUT_RESET_OUT);
			addOutput(outPortWidgets[Meander::OUT_RESET_OUT]);
					
			outPortWidgets[Meander::OUT_MELODY_VOLUME_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(266.982, 107.333)), module, Meander::OUT_MELODY_VOLUME_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_MELODY_VOLUME_OUTPUT]);

			outPortWidgets[Meander::OUT_HARMONY_VOLUME_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(201.789, 107.616)), module, Meander::OUT_HARMONY_VOLUME_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_HARMONY_VOLUME_OUTPUT]);

			outPortWidgets[Meander::OUT_BASS_VOLUME_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(325.12, 107.616)), module, Meander::OUT_BASS_VOLUME_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_BASS_VOLUME_OUTPUT]);
			
			outPortWidgets[Meander::OUT_MELODY_CV_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(266.899, 115.813)), module, Meander::OUT_MELODY_CV_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_MELODY_CV_OUTPUT]);
			
			outPortWidgets[Meander::OUT_BASS_CV_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(325.266, 115.817)), module, Meander::OUT_BASS_CV_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_BASS_CV_OUTPUT]);

			outPortWidgets[Meander::OUT_BASS_NOTE2_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(325.266, 115.817)), module, Meander::OUT_BASS_NOTE2_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_BASS_NOTE2_OUTPUT]);

			outPortWidgets[Meander::OUT_HARMONY_CV_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(202.176, 115.909)), module, Meander::OUT_HARMONY_CV_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_HARMONY_CV_OUTPUT]);

			outPortWidgets[Meander::OUT_HARMONY_NOTE2_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(202.176, 115.909)), module, Meander::OUT_HARMONY_NOTE2_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_HARMONY_NOTE2_OUTPUT]);
			outPortWidgets[Meander::OUT_HARMONY_NOTE3_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(202.176, 115.909)), module, Meander::OUT_HARMONY_NOTE3_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_HARMONY_NOTE3_OUTPUT]);
			outPortWidgets[Meander::OUT_HARMONY_NOTE4_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(202.176, 115.909)), module, Meander::OUT_HARMONY_NOTE4_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_HARMONY_NOTE4_OUTPUT]);

			outPortWidgets[Meander::OUT_CLOCK_BEATX2_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(78.74, 122.291)), module, Meander::OUT_CLOCK_BEATX2_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_CLOCK_BEATX2_OUTPUT]);

			outPortWidgets[Meander::OUT_CLOCK_BAR_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(37.143, 122.537)), module, Meander::OUT_CLOCK_BAR_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_CLOCK_BAR_OUTPUT]);

			outPortWidgets[Meander::OUT_CLOCK_BEATX4_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(99.342, 122.573)), module, Meander::OUT_CLOCK_BEATX4_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_CLOCK_BEATX4_OUTPUT]);

			outPortWidgets[Meander::OUT_CLOCK_BEATX8_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(121.073, 122.573)), module, Meander::OUT_CLOCK_BEATX8_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_CLOCK_BEATX8_OUTPUT]);

			outPortWidgets[Meander::OUT_CLOCK_BEAT_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(57.856, 122.856)), module, Meander::OUT_CLOCK_BEAT_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_CLOCK_BEAT_OUTPUT]);

			outPortWidgets[Meander::OUT_BASS_GATE_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(325.402, 123.984)), module, Meander::OUT_BASS_GATE_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_BASS_GATE_OUTPUT]);

			outPortWidgets[Meander::OUT_BASS_GATE2_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(325.402, 123.984)), module, Meander::OUT_BASS_GATE2_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_BASS_GATE2_OUTPUT]);

			outPortWidgets[Meander::OUT_HARMONY_GATE_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(202.071, 124.267)), module, Meander::OUT_HARMONY_GATE_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_HARMONY_GATE_OUTPUT]);

			outPortWidgets[Meander::OUT_HARMONY_GATE4_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(202.071, 124.267)), module, Meander::OUT_HARMONY_GATE4_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_HARMONY_GATE4_OUTPUT]);

			outPortWidgets[Meander::OUT_MELODY_GATE_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(266.982, 124.549)), module, Meander::OUT_MELODY_GATE_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_MELODY_GATE_OUTPUT]);

			outPortWidgets[Meander::OUT_FBM_HARMONY_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(380.0, 107.616)), module, Meander::OUT_FBM_HARMONY_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_FBM_HARMONY_OUTPUT]);

			outPortWidgets[Meander::OUT_FBM_MELODY_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(380.0, 115.815)), module, Meander::OUT_FBM_MELODY_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_FBM_MELODY_OUTPUT]);

			outPortWidgets[Meander::OUT_FBM_ARP_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(380.0, 124.831)), module, Meander::OUT_FBM_ARP_OUTPUT);
			addOutput(outPortWidgets[Meander::OUT_FBM_ARP_OUTPUT]);

			// outPortWidgets[Meander::OUT_EXT_POLY_SCALE_OUTPUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(380.0, 124.831)), module, Meander::OUT_EXT_POLY_SCALE_OUTPUT);
			// addOutput(outPortWidgets[Meander::OUT_EXT_POLY_SCALE_OUTPUT]);

			outPortWidgets[Meander::OUT_CLOCK_OUT]=createOutputCentered<PJ301MPort>(mm2px(Vec(45.0, 350.0)), module, Meander::OUT_CLOCK_OUT);
			addOutput(outPortWidgets[Meander::OUT_CLOCK_OUT]);

									
			//**********************************

			// now, procedurally rearrange the control param panel locations
			Vec CircleCenter= Vec(mm2px(116.75),mm2px(67.75));
			float OuterCircleRadius=mm2px(39);
			float InnerCircleRadius3=mm2px(26);
					
			// re-layout the circle BUTTONS and LIGHTS programatically  // does not draw circles and segments and text.  That is done by drawCircleOf5ths()
			for(int i = 0; i<MAX_NOTES;++i) 
			{
				const float rotate90 = (M_PI) / 2.0;
							
				double startDegree = (M_PI * 2.0 * ((double)i - 0.5) / MAX_NOTES) - rotate90;
				double endDegree = (M_PI * 2.0 * ((double)i + 0.5) / MAX_NOTES) - rotate90;
				double x1= cos(startDegree) * InnerCircleRadius3 + CircleCenter.x;
				double y1= sin(startDegree) * InnerCircleRadius3 + CircleCenter.y;
				double x2= cos(endDegree) * InnerCircleRadius3 + CircleCenter.x;
				double y2= sin(endDegree) * InnerCircleRadius3 + CircleCenter.y;
				Vec radialLine1=Vec(x1,y1).minus(CircleCenter);
				Vec radialLine2=Vec(x2,y2).minus(CircleCenter);
				Vec centerLine=(radialLine1.plus(radialLine2)).div(2.);
				
				Vec radialDirection=centerLine.normalize();

								
				Vec controlPosition=CircleCenter.plus(radialDirection.mult(OuterCircleRadius*.78f));
				controlPosition=controlPosition.minus((paramWidgets[Meander::BUTTON_CIRCLESTEP_C_PARAM+i]->box.size).div(2.));  // adjust for box size
				paramWidgets[Meander::BUTTON_CIRCLESTEP_C_PARAM+i]->box.pos=controlPosition;
				
				controlPosition=CircleCenter.plus(radialDirection.mult(OuterCircleRadius*.78f));
				controlPosition=controlPosition.minus((lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_1+i]->box.size).div(2.));  // adjust for box size
				lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESTEP_1+i]->box.pos=controlPosition;
				

				controlPosition=CircleCenter.plus(radialDirection.mult(OuterCircleRadius*.61f));
				controlPosition=controlPosition.minus((lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT+i]->box.size).div(2.));
				lightWidgets[Meander::LIGHT_CIRCLE_ROOT_KEY_POSITION_1_LIGHT+i]->box.pos=controlPosition;
			}
 
		
			// re-layout the circle step set buttons and lights programatically
			Vec start_position=Vec(0,0);
			float verticalIncrement=mm2px(5.92f);
			for(int i = 0; i<MAX_STEPS;++i) 
			{
				start_position=mm2px(Vec(62, 128.9- 109.27));  // for Y subtract SVG Y from panel height 128.9 and then convert to px
				Vec buttonPosition=start_position.plus(Vec(0, (i*verticalIncrement)));
				buttonPosition.y -= paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_1_PARAM+i]->box.size.y;  // adjust for box height
				paramWidgets[Meander::BUTTON_HARMONY_SETSTEP_1_PARAM+i]->box.pos=buttonPosition;
				start_position=mm2px(Vec(63.5, 128.9- 110.8));  // for Y subtract SVG Y from panel height 128.9 and then convert to px
				buttonPosition=start_position.plus(Vec(0, (i*verticalIncrement)));
				buttonPosition.y -= lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_1+i]->box.size.y; // adjust for box height
				lightWidgets[Meander::LIGHT_LEDBUTTON_CIRCLESETSTEP_1+i]->box.pos=buttonPosition;
			}


			// relayout all param controls and lights
		
			Vec drawCenter=Vec(5., 30.);
			
			// do upper left controls and ports
			drawCenter=drawCenter.plus(Vec(37,0));
			paramWidgets[Meander::BUTTON_RUN_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_RUN_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_RUN]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_RUN]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(37,0));
			outPortWidgets[Meander::OUT_RUN_OUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_RUN_OUT]->box.size.div(2.));
			
			drawCenter=drawCenter.minus(Vec(37,0)); 
			drawCenter=drawCenter.plus(Vec(0,40));
	
			paramWidgets[Meander::BUTTON_RESET_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_RESET_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_RESET]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_RESET]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(37,0));
			outPortWidgets[Meander::OUT_RESET_OUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_RESET_OUT]->box.size.div(2.));

			drawCenter=Vec(42., 110.);
			
			paramWidgets[Meander::CONTROL_TEMPOBPM_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_TEMPOBPM_PARAM]->box.size.div(2.));
				
			drawCenter=drawCenter.plus(Vec(0,25));
			
			paramWidgets[Meander::CONTROL_TIMESIGNATURETOP_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_TIMESIGNATURETOP_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,25));
			paramWidgets[Meander::CONTROL_TIMESIGNATUREBOTTOM_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_TIMESIGNATUREBOTTOM_PARAM]->box.size.div(2.));

			drawCenter=drawCenter.plus(Vec(0,25));
			paramWidgets[Meander::CONTROL_ROOT_KEY_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_ROOT_KEY_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,25));
			paramWidgets[Meander::CONTROL_SCALE_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_SCALE_PARAM]->box.size.div(2.));
 
								
			// redo all harmony controls positions
			drawCenter=Vec(512., 40.);
	
			paramWidgets[Meander::BUTTON_ENABLE_HARMONY_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_HARMONY_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_HARMONY_ENABLE]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_HARMONY_ENABLE]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));	
			paramWidgets[Meander::CONTROL_HARMONY_VOLUME_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONY_VOLUME_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(-20,22));
			inPortWidgets[Meander::IN_HARMONY_STEPS_EXT_CV]->box.pos=drawCenter.minus(inPortWidgets[Meander::IN_HARMONY_STEPS_EXT_CV]->box.size.div(2.));	
			drawCenter=drawCenter.plus(Vec(20,0));
			paramWidgets[Meander::CONTROL_HARMONY_STEPS_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONY_STEPS_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));		
			paramWidgets[Meander::CONTROL_HARMONY_TARGETOCTAVE_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONY_TARGETOCTAVE_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));			
			paramWidgets[Meander::CONTROL_HARMONY_ALPHA_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONY_ALPHA_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_HARMONY_RANGE_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONY_RANGE_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));			  
			paramWidgets[Meander::CONTROL_HARMONY_DIVISOR_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONY_DIVISOR_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));			
			paramWidgets[Meander::BUTTON_ENABLE_HARMONY_ALL7THS_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_HARMONY_ALL7THS_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_ALL7THS_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_ALL7THS_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(85,0));
			paramWidgets[Meander::BUTTON_ENABLE_HARMONY_V7THS_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_HARMONY_V7THS_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_V7THS_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_V7THS_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(-85,22));			
			paramWidgets[Meander::BUTTON_ENABLE_HARMONY_STACCATO_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_HARMONY_STACCATO_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_STACCATO_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_HARMONY_STACCATO_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));	
			paramWidgets[Meander::CONTROL_HARMONYPRESETS_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONYPRESETS_PARAM]->box.size.div(2.));

			// redo all melody controls positions

			drawCenter=Vec(710., 40.);

			paramWidgets[Meander::BUTTON_ENABLE_MELODY_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_MELODY_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_MELODY_VOLUME_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_MELODY_VOLUME_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_MELODY_NOTE_LENGTH_DIVISOR_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_MELODY_TARGETOCTAVE_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_MELODY_TARGETOCTAVE_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_MELODY_ALPHA_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_MELODY_ALPHA_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_MELODY_RANGE_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_MELODY_RANGE_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::BUTTON_MELODY_DESTUTTER_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_MELODY_DESTUTTER_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_DESTUTTER]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_DESTUTTER]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(90,0));
			paramWidgets[Meander::BUTTON_ENABLE_MELODY_STACCATO_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_MELODY_STACCATO_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_MELODY_STACCATO_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_MELODY_STACCATO_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(-90,22));
			paramWidgets[Meander::BUTTON_ENABLE_MELODY_CHORDAL_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_MELODY_CHORDAL_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_CHORDAL]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_CHORDAL]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(90,0));
			paramWidgets[Meander::BUTTON_ENABLE_MELODY_SCALER_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_MELODY_SCALER_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_SCALER]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_MELODY_ENABLE_SCALER]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(-90,0));

			// arp

			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::BUTTON_ENABLE_ARP_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_ARP_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_ARP_COUNT_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_ARP_COUNT_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_ARP_INCREMENT_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_ARP_INCREMENT_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_ARP_DECAY_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_ARP_DECAY_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_ARP_PATTERN_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_ARP_PATTERN_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::BUTTON_ENABLE_ARP_CHORDAL_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_ARP_CHORDAL_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE_CHORDAL]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE_CHORDAL]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(90,0));
			paramWidgets[Meander::BUTTON_ENABLE_ARP_SCALER_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_ARP_SCALER_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE_SCALER]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_ARP_ENABLE_SCALER]->box.size.div(2.));
							
			drawCenter=Vec(900., 130.);
		
			paramWidgets[Meander::BUTTON_ENABLE_BASS_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_BASS_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_ENABLE]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_ENABLE]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_BASS_VOLUME_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_BASS_VOLUME_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_BASS_TARGETOCTAVE_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_BASS_TARGETOCTAVE_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_BASS_DIVISOR_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_BASS_DIVISOR_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::BUTTON_ENABLE_BASS_STACCATO_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_ENABLE_BASS_STACCATO_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_BASS_STACCATO_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_ENABLE_BASS_STACCATO_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::BUTTON_BASS_ACCENT_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_BASS_ACCENT_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_ACCENT_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_ACCENT_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::BUTTON_BASS_SYNCOPATE_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_BASS_SYNCOPATE_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_SYNCOPATE_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_SYNCOPATE_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::BUTTON_BASS_SHUFFLE_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_BASS_SHUFFLE_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_SHUFFLE_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_SHUFFLE_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::BUTTON_BASS_OCTAVES_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_BASS_OCTAVES_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_OCTAVES_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_BASS_OCTAVES_PARAM]->box.size.div(2.));
			
			// fBm controls

			drawCenter=Vec(1055., 150.);

			paramWidgets[Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONY_FBM_OCTAVES_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_HARMONY_FBM_PERIOD_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_HARMONY_FBM_PERIOD_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,33));
			paramWidgets[Meander::CONTROL_MELODY_FBM_OCTAVES_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_MELODY_FBM_OCTAVES_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_MELODY_FBM_PERIOD_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_MELODY_FBM_PERIOD_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,33));
			paramWidgets[Meander::CONTROL_ARP_FBM_OCTAVES_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_ARP_FBM_OCTAVES_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));
			paramWidgets[Meander::CONTROL_ARP_FBM_PERIOD_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::CONTROL_ARP_FBM_PERIOD_PARAM]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(0,22));

		
			drawCenter=Vec(345., 250.);
			paramWidgets[Meander::BUTTON_PROG_STEP_PARAM]->box.pos=drawCenter.minus(paramWidgets[Meander::BUTTON_PROG_STEP_PARAM]->box.size.div(2.));
			lightWidgets[Meander::LIGHT_LEDBUTTON_PROG_STEP_PARAM]->box.pos=drawCenter.minus(lightWidgets[Meander::LIGHT_LEDBUTTON_PROG_STEP_PARAM]->box.size.div(2.));
		
			// re-layout all input ports.  Work around parm and input enum value mismatch due to history
			for (int i=0; i<Meander::NUM_INPUTS; ++i)
			{
				if (i<=Meander::IN_SCALE_EXT_CV)
				{
					if ((inPortWidgets[i]!=NULL)&&(paramWidgets[i]!=NULL))
						inPortWidgets[i]->box.pos= paramWidgets[i]->box.pos.minus(Vec(20,-1));
				}
				else
				if (i==Meander::IN_CLOCK_EXT_CV)
				{
					Vec drawCenter=Vec(20., 335.);  // raise panel vertical position a bit to not conflict with CPU meter display
					inPortWidgets[Meander::IN_CLOCK_EXT_CV]->box.pos=drawCenter.minus(inPortWidgets[Meander::IN_CLOCK_EXT_CV]->box.size.div(2.));
				}
				else
				if (i==Meander::IN_HARMONY_CIRCLE_DEGREE_EXT_CV)
				{
					Vec drawCenter=Vec(345., 210.);
					inPortWidgets[Meander::IN_HARMONY_CIRCLE_DEGREE_EXT_CV]->box.pos=drawCenter.minus(inPortWidgets[Meander::IN_HARMONY_CIRCLE_DEGREE_EXT_CV]->box.size.div(2.));
				}
				else
				if (i==Meander::IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV)
				{
					Vec drawCenter=Vec(345., 230.);
					inPortWidgets[Meander::IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV]->box.pos=drawCenter.minus(inPortWidgets[Meander::IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV]->box.size.div(2.));
				}
				else
				if (i==Meander::IN_MELODY_SCALE_DEGREE_EXT_CV)
				{
					Vec drawCenter=Vec(512.+290, 31.);
					inPortWidgets[Meander::IN_MELODY_SCALE_DEGREE_EXT_CV]->box.pos=drawCenter.minus(inPortWidgets[Meander::IN_MELODY_SCALE_DEGREE_EXT_CV]->box.size.div(2.));
				}
				else
				if (i==Meander::IN_MELODY_SCALE_GATE_EXT_CV)
				{
					Vec drawCenter=Vec(512.+290, 47.);
					inPortWidgets[Meander::IN_MELODY_SCALE_GATE_EXT_CV]->box.pos=drawCenter.minus(inPortWidgets[Meander::IN_MELODY_SCALE_GATE_EXT_CV]->box.size.div(2.));
				}
				else
				{
					int parmIndex=Meander::BUTTON_ENABLE_MELODY_PARAM+i-Meander::IN_HARMONY_CIRCLE_DEGREE_GATE_EXT_CV-1;
					if ((inPortWidgets[i]!=NULL)&&(paramWidgets[parmIndex]!=NULL))
						inPortWidgets[i]->box.pos= paramWidgets[parmIndex]->box.pos.minus(Vec(20,-1));
				}
				
			}
			

			// re-layout all output port

			drawCenter=Vec(520., 365.);

			outPortWidgets[Meander::OUT_HARMONY_CV_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_HARMONY_CV_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(60,0));
			outPortWidgets[Meander::OUT_HARMONY_GATE_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_HARMONY_GATE_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(60,0));
			outPortWidgets[Meander::OUT_HARMONY_VOLUME_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_HARMONY_VOLUME_OUTPUT]->box.size.div(2.));

			outPortWidgets[Meander::OUT_HARMONY_NOTE2_OUTPUT]->box.pos=outPortWidgets[Meander::OUT_HARMONY_CV_OUTPUT]->box.pos.plus(Vec(0, -45));
			outPortWidgets[Meander::OUT_HARMONY_NOTE3_OUTPUT]->box.pos=outPortWidgets[Meander::OUT_HARMONY_CV_OUTPUT]->box.pos.plus(Vec(35, -45));
			outPortWidgets[Meander::OUT_HARMONY_NOTE4_OUTPUT]->box.pos=outPortWidgets[Meander::OUT_HARMONY_CV_OUTPUT]->box.pos.plus(Vec(35*2, -45));
			outPortWidgets[Meander::OUT_HARMONY_GATE4_OUTPUT]->box.pos=outPortWidgets[Meander::OUT_HARMONY_CV_OUTPUT]->box.pos.plus(Vec(35*3, -45));

			drawCenter=Vec(717., 365.);
			outPortWidgets[Meander::OUT_MELODY_CV_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_MELODY_CV_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(60,0));
			outPortWidgets[Meander::OUT_MELODY_GATE_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_MELODY_GATE_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(60,0));
			outPortWidgets[Meander::OUT_MELODY_VOLUME_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_MELODY_VOLUME_OUTPUT]->box.size.div(2.));

			drawCenter=Vec(895., 365.);
			outPortWidgets[Meander::OUT_BASS_CV_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_BASS_CV_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(50,0));
			outPortWidgets[Meander::OUT_BASS_GATE_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_BASS_GATE_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(50,0));
			outPortWidgets[Meander::OUT_BASS_VOLUME_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_BASS_VOLUME_OUTPUT]->box.size.div(2.));

			outPortWidgets[Meander::OUT_BASS_GATE2_OUTPUT]->box.pos=outPortWidgets[Meander::OUT_BASS_VOLUME_OUTPUT]->box.pos.plus(Vec(0, -45));
			outPortWidgets[Meander::OUT_BASS_NOTE2_OUTPUT]->box.pos=outPortWidgets[Meander::OUT_BASS_VOLUME_OUTPUT]->box.pos.plus(Vec(-35, -45));

			drawCenter=Vec(1060., 365.);
			outPortWidgets[Meander::OUT_FBM_HARMONY_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_FBM_HARMONY_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(50,0));
			outPortWidgets[Meander::OUT_FBM_MELODY_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_FBM_MELODY_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(50,0));
			outPortWidgets[Meander::OUT_FBM_ARP_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_FBM_ARP_OUTPUT]->box.size.div(2.));
			
			//

			drawCenter=Vec(100., 365.);
			outPortWidgets[Meander::OUT_CLOCK_BAR_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_CLOCK_BAR_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(40,0));
			outPortWidgets[Meander::OUT_CLOCK_BEAT_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_CLOCK_BEAT_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(40,0));
			outPortWidgets[Meander::OUT_CLOCK_BEATX2_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_CLOCK_BEATX2_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(40,0));
			outPortWidgets[Meander::OUT_CLOCK_BEATX4_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_CLOCK_BEATX4_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(40,0));
			outPortWidgets[Meander::OUT_CLOCK_BEATX8_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_CLOCK_BEATX8_OUTPUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(40,0));

			// drawCenter=Vec(145., 300.);
			// outPortWidgets[Meander::OUT_EXT_POLY_SCALE_OUTPUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_EXT_POLY_SCALE_OUTPUT]->box.size.div(2.));
			// drawCenter=drawCenter.plus(Vec(40,0));
		
			drawCenter=Vec(60., 350.);  // adjust the port a bit to right to avoid CPU meter display
			outPortWidgets[Meander::OUT_CLOCK_OUT]->box.pos=drawCenter.minus(outPortWidgets[Meander::OUT_CLOCK_OUT]->box.size.div(2.));
			drawCenter=drawCenter.plus(Vec(40,0));

			//********************
			
			for (int i=0; ((i<Meander::NUM_PARAMS)&&(i<MAX_PARAMS)); ++i)  // get the paramWidget box into a MeanderWidget array so it can be accessed as needed
			{
				if (paramWidgets[i]!=NULL) 
					ParameterRect[i]=paramWidgets[i]->box;
			}

			for (int i=0; ((i<Meander::NUM_OUTPUTS)&&(i<MAX_OUTPORTS)); ++i)  // get the paramWidget box into a MeanderWidget array so it can be accessed as needed
			{
				if (outPortWidgets[i]!=NULL) 
					OutportRect[i]=outPortWidgets[i]->box;
			}

			for (int i=0; ((i<Meander::NUM_INPUTS)&&(i<MAX_INPORTS)); ++i)  // get the paramWidget box into a MeanderWidget array so it can be accessed as needed
			{
				if (inPortWidgets[i]!=NULL) 
					InportRect[i]=inPortWidgets[i]->box;
			}
				
		
		}  // end if (true) line 5336

	}    // end MeanderWidget(Meander* module)  
 
			
	void step() override   // note, this is a widget step() which is not deprecated and is a GUI call.  This advances UI by one "frame"
	{  
		Meander *module = dynamic_cast<Meander*>(this->module);  // some plugins do this
		if(module == NULL) return;
	
	   	if ((module != NULL)&&(module->instanceRunning))  
		{ 
			if (module->theMeanderState.updateDisplay)
			{
				module->theMeanderState.updateDisplay = false;
				display->dirty = true;
			}
			auto wire = api0::gRackWidget->wireContainer->getTopWire(inPortWidgets[Meander::IN_PROG_STEP_EXT_CV]);
			if (wire && wire->outputPort->module == this->module)
				module->theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port = wire->outputPort->portId;
			else 
				module->theMeanderState.theHarmonyParms.STEP_inport_connected_to_Meander_trigger_port=0;
		}
	
		ModuleWidget::step();
	} // end step()

};  // end struct MeanderWidget



Model* modelMeander = createModel<Meander, MeanderWidget>("Meander");


