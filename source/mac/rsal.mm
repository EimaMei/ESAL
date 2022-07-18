#import "../../include/ESAL.hpp"
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include <unistd.h>

struct audioFile {
    AVAudioEngine* audioEngine;
	AVAudioPlayerNode* playerNode;
	AVAudioTime *playerTime;
	ESAL::audioArgs arg;
	NSDate* start;
};
std::vector<audioFile> list;
int num_of_songs=0;

int setupAudio(std::string f, ESAL::audioArgs arg, ESAL::time& old, int num, bool seek=false);
void* fading_in(void* argument);
void* fading_out(void* argument);

ESAL::audio::audio(std::string file, audioArgs a/*={}*/) { 
	audio::file=file;
	arg=a;
	if (access(file.c_str(), F_OK) == -1) { 
		error_code=1;
	}
	else {
		int s = setupAudio(audio::file=file, arg, old, num);
		
		//ESAL::track m(file);track=m;
		track.duration = secondsToTime(s);
		inf_loops=(arg.loops == -1);
		num=num_of_songs;
		num_of_songs++;

		if (old.m > s)                { error_code=2;  }
		else if (old.h > s)           { error_code=3;  }
		else if (arg.speed > 4)       { error_code=4;  } // https://developer.apple.com/documentation/avfaudio/avaudiounitvarispeed/1387118-rate?language=objc
		else if (arg.speed < 0.25)    { error_code=5;  }
		else if (arg.loops < -1)      { error_code=6;  } 
		else if (arg.pitch > 2400)    { error_code=7;  } // https://developer.apple.com/documentation/avfaudio/avaudiounittimepitch/1387188-pitch?language=objc
		else if (arg.pitch < -2400)   { error_code=8;  }
		else if (arg.volume > 100)    { error_code=9;  } // https://developer.apple.com/documentation/avfaudio/avaudiomixing/1387422-volume?language=objc
		else if (arg.volume < 0)      { error_code=10; }
		else if (arg.fadeIn > s)     { error_code=11; }
		else if (arg.fadeOut > s)    { error_code=12; }
		else if (arg.deadSpace < -1) { error_code=13; } 
	}
	
}

bool ESAL::audio::isPlaying() {	
	//if (error_code == 1) { return false;}
	AVAudioPlayerNode* playerNode = list[num].playerNode;
	NSDate* end = [NSDate date];

	if (int([end timeIntervalSinceDate:list[num].start]) >= 1 && function != 3) {
		list[num].start = [NSDate date];
		list[num].playerTime=playerNode.lastRenderTime;
	}
	AVAudioTime *playerTime = list[num].playerTime;
	
	if (function==0 && arg.fadeIn > 0) {
		pthread_t t;
    	pthread_create(&t,NULL, fading_in, &list[num]);
		function=1;
	}

    int s = round(double(playerTime.sampleTime / playerTime.sampleRate))-old.s+old.m; 
	if (s < -1 || function == 3) { 
		if (arg.deadSpace <= 0) { done=true; return false; }
		else { done=true;  }
	} 
	else {
		current = secondsToTime(s);
		int track_pos = timeToSeconds(track.duration);

		done = ( track_pos <= s );
		if (arg.fadeOut > 0) {
			int fadeOutpos=s+arg.fadeOut+1;
			if ( track_pos<=fadeOutpos && function != 2) {
				pthread_t t;
    			pthread_create(&t,NULL, fading_out, &list[num]);
				function=2;
			}
		}
	}

	if (done) { 
		if (arg.loops > 0 || inf_loops) {
            if (!inf_loops) arg.loops--;
            arg.end = {};
            seekTo({0,0,0});
			done=false;
			function = -2;
		}
		else { old.s+=s; }
		old.m=0;
		current = {};
		if (arg.deadSpace > 0 && function != 3) {
			list[num].start = [NSDate date]; // requires retweaking
			function=3;
			done=false;
		}
		else if (arg.deadSpace > 0) {
			NSDate* end = [NSDate date];
			int d = [end timeIntervalSinceDate:list[num].start];
			
			if (d < arg.deadSpace) { done=false; }
		}
		else { function=0; }
	}
	usleep(1*1000);

    return !done;
}

int ESAL::audio::play() {
	if (error_code == 1) return -1;
	AVAudioPlayerNode* playerNode = list[num].playerNode;

    [playerNode play];

	return 0;
}

int ESAL::audio::pause() {
	if (error_code == 1) return -1;
	AVAudioPlayerNode* playerNode = list[num].playerNode;
	[playerNode pause];

	return 0;
}

int ESAL::audio::resume(){
	if (error_code == 1) return -1;
	AVAudioPlayerNode* playerNode = list[num].playerNode;
	[playerNode play];

	return 0;
}

int ESAL::audio::stop(){
	if (error_code == 1) return -1;

	[list[num].playerNode release];
	[list[num].audioEngine release];
	[list[num].playerTime release];
	[list[num].start release];
	list[num] = {};
	

	done=true;

	return 0;
}


int ESAL::audio::seekTo(time t) {
	if (error_code == 1) return -1;
	else if (timeToSeconds(t) > timeToSeconds(track.duration)) { arg.start=t; error_code = 14; return -1; }

	stop();
	arg.start=t;

	setupAudio(ESAL::audio::file, arg, old, num, true);
	play(); 

	return 0;
}

int ESAL::audio::seekTo(const char* format) {
	if (error_code == 1) return -1;
	NSArray *array = [[NSString stringWithUTF8String:format] componentsSeparatedByString:@":"];
	int size = [array count];

	time t;
	// if size 1, then seconds
	// if size 2, then minutes and seconds
	// if size 3+, then hours, minutes and seconds
	switch (size) {
		case 0: 
		return -1;

		case 1: 
		t = {0,0, [[array[0] description] intValue]}; break;

		case 2: 
		t = {0,[[array[0] description] intValue], [[array[1] description] intValue]}; break;

		default: 
		t = {[[array[0] description] intValue], [[array[1] description] intValue], [[array[2] description] intValue]};
	}
	[array release];
	
	return seekTo(t);
}

int ESAL::audio::checkEvents() { 
	if (error_code == 1) return -1;
	isPlaying();

	return 0;
}

int setupAudio(std::string f, ESAL::audioArgs arg, ESAL::time& old, int num, bool seek) {
	AVAudioEngine* audioEngine = [[AVAudioEngine alloc] init];
	AVAudioPlayerNode* playerNode = [[AVAudioPlayerNode alloc] init];
	AVAudioUnitVarispeed* speedNode = [[AVAudioUnitVarispeed alloc] init];
	AVAudioMixerNode* mixerNode = [[AVAudioMixerNode alloc] init];
	AVAudioUnitTimePitch* pitchNode = [[AVAudioUnitTimePitch alloc] init];

	NSArray *listItems = [[NSString stringWithUTF8String:f.c_str()] componentsSeparatedByString:@"."];
	NSURL* url = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:listItems[0] ofType:listItems[1]]];
	AVAudioFile* audioFile= [[AVAudioFile alloc] initForReading:url error:nil];

	if (arg.correctPitch) { arg.pitch = -1200 * log2(arg.speed); }
	speedNode.rate = arg.speed;
	pitchNode.pitch= arg.pitch;

	if (arg.fadeIn > 0 && !seek) { playerNode.volume=0; }
	else { playerNode.volume = arg.volume/100.0; }
	mixerNode.outputVolume = 1;

	AVAudioFormat* outputFormat = [playerNode outputFormatForBus:0];
	AVAudioFormat* khznotnode = [khznotnode initWithCommonFormat:outputFormat.commonFormat sampleRate:arg.sampleRate channels: outputFormat.channelCount interleaved:outputFormat.interleaved];

	[audioEngine attachNode:playerNode];
	[audioEngine attachNode:mixerNode];
	[audioEngine attachNode:speedNode];
	[audioEngine attachNode:pitchNode];

	[audioEngine connect:playerNode to:mixerNode format:khznotnode];
	[audioEngine connect:mixerNode to:speedNode format:khznotnode];
	[audioEngine connect:speedNode to:pitchNode format:khznotnode];
	[audioEngine connect:pitchNode to:audioEngine.outputNode format:khznotnode];

	old.m= timeToSeconds(arg.start);
	old.h = timeToSeconds(arg.end);

	AVAudioFramePosition startTime;
	AVAudioFramePosition endTime;

	startTime = playerNode.lastRenderTime.sampleTime + (old.m * outputFormat.sampleRate);
	if (old.h <= 0) endTime = playerNode.lastRenderTime.sampleTime + (audioFile.length * outputFormat.sampleRate);
	else endTime = playerNode.lastRenderTime.sampleTime + (old.h * outputFormat.sampleRate);

	[playerNode scheduleSegment:audioFile startingFrame:startTime frameCount:endTime atTime: nil completionHandler:nil];
	[audioEngine startAndReturnError:nil];
	NSDate* start = [NSDate date];

	if (list.size() < num+1) list.insert(list.end(), {audioEngine, playerNode, playerNode.lastRenderTime, arg, start});
	else list[num] = {audioEngine, playerNode, playerNode.lastRenderTime, arg, start};

	int s = round(double(audioFile.length) / double(audioFile.processingFormat.sampleRate));

	[speedNode release];
	[mixerNode release];
	[pitchNode release];
	[outputFormat release];
	[audioFile release];
	[start release];
	[listItems release];
	[url release];
	[khznotnode release];

	return s;
}

std::string ESAL::audio::getError() {
	switch (error_code) {
		case 0:  return "No errors found.";
		case 1:  return "Failed to open file '"+file+"' (file either doesn't exist or the program has no permissions to open the file)";
		case 2:  return "Argument '.start' length ("+std::to_string(old.m)+" seconds) is too high compared to the audio file's length ("+std::to_string(timeToSeconds(track.duration))+" seconds)";
		case 3:  return "Argument '.end' length ("+std::to_string(old.h)+" seconds) is too high compared to the audio file's length ("+std::to_string(timeToSeconds(track.duration))+" seconds)";
		case 4:  return "Argument '.speed' value ("+std::to_string(arg.speed)+" speed) is too high compared to the highest speed value supported (4 speed)";
		case 5:  return "Argument '.speed' value ("+std::to_string(arg.speed)+" speed) is too low compared to the lowest speed value supported (0.25 speed)";
		case 6:  return "Argument '.loops' value ("+std::to_string(arg.loops)+" loops) is too low compared to the lowest loop value supported (1+ loops allowed while -1 is infinite loops)";
		case 7:  return "Argument '.pitch' value ("+std::to_string(arg.pitch)+" pitch) is too high compared to the highest pitch value supported (2400 pitch)";
		case 8:  return "Argument '.pitch' value ("+std::to_string(arg.pitch)+" pitch) is too low compared to the lowest pitch value supported (-2400 pitch)";
		case 9:  return "Argument '.volume' value ("+std::to_string(arg.volume)+" volume) is too high compared to the highest volume value supported (100 volume)";
		case 10: return "Argument '.volume' value ("+std::to_string(arg.volume)+" volume) is too low compared to the lowest volume value supported (0 volume)";
		case 11: return "Argument '.fadeIn' value ("+std::to_string(arg.fadeIn)+" seconds) is too high compared to the audio file's length ("+std::to_string(timeToSeconds(track.duration))+" seconds)";
		case 12: return "Argument '.fadeOut' value ("+std::to_string(arg.fadeOut)+" seconds) is too high compared to the audio file's length ("+std::to_string(timeToSeconds(track.duration))+" seconds)";
		case 13: return "Argument '.deadSpace' value ("+std::to_string(arg.deadSpace)+" seconds) is too low compared to the lowest deadSpace value supported (0 seconds)";
		case 14: return "Argument 'time' ("+std::to_string(timeToSeconds(arg.start))+" seconds) at function .seekTo() is too high compared to the audio file's length ("+std::to_string(timeToSeconds(track.duration))+" seconds)";
		default: return "Undocumented error code '"+std::to_string(error_code)+"'. Check the source code of this ESAL version for more information";
	}
	return "N/A";
}

void ESAL::quit() {
	for (int num=0; num<list.size(); num++){
		[list[num].playerNode release];
		[list[num].audioEngine release];
		[list[num].playerTime release];
		[list[num].start release];
		list[num] = {};
	}
}

void* fading_in(void* argument) {
	audioFile* a = (audioFile*)argument;
	AVAudioPlayerNode* playerNode= a->playerNode;
	ESAL::audioArgs arg = a->arg;

	while (playerNode.volume < 1) {
		usleep(arg.fadeIn*10*1000);
		playerNode.volume+=0.01;
	}
	return 0;
}

void* fading_out(void* argument) {
	audioFile* a = (audioFile*)argument;
	AVAudioPlayerNode* playerNode= a->playerNode;
	ESAL::audioArgs arg = a->arg;

	while (playerNode.volume > 0) {
		usleep(arg.fadeOut*10*1000);
		playerNode.volume-=0.01;
	}

	return 0;
}