#include <raylib.h>
#include <stddef.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>


#define WIDTH 					  600
#define HEIGHT					  800


#define MAX_VOICES 				  	3
#define MAX_SAMPLES               512
#define MAX_SAMPLES_PER_UPDATE   4096


float tempered_frequencies[12] = {
    110.00f, 116.54f, 123.47f, 130.81f, 138.59f, 146.83f, // A2, A#2, B2, C3, C#3, D3
    155.56f, 164.81f, 174.61f, 185.00f, 196.00f, 207.65f  // D#3, E3, F3, F#3, G3, G#3
};

char notes[12][4] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

typedef struct
{
	int idx;
	// Cycles per second (hz)
	float frequency;
	float baseFrequency;
	// Audio frequency, for smoothing
	float audioFrequency;
	// Previous value, used to test if sine needs to be rewritten, and to smoothly modulate frequency
	float oldFrequency;
	// Index for audio rendering
	float sineIdx;
}Voice;

Voice voices[3];

float getFreqFromIdx(size_t i)
{
	return (tempered_frequencies[voices[i].idx])* pow(2, 2);
}

// Audio input processing callback
void AudioInputCallback(void *buffer, unsigned int frames)
{
	for(size_t i = 0; i < MAX_VOICES; i++)
	{
    	// voices[i].audioFrequency = (voices[i].baseFrequency + voices[i].frequency) + (voices[i].audioFrequency - (voices[i].baseFrequency + voices[i].frequency))*0.95f;
		voices[i].audioFrequency = getFreqFromIdx(i);
	
    	float incr = voices[i].audioFrequency/44100.0f;
    	short *d = (short *)buffer;
	
    	for (unsigned int j = 0; j < frames; j++)
    	{
    	    d[j] += (short)(32000.0f/MAX_VOICES*sinf(2*PI*voices[i].sineIdx));
    	    voices[i].sineIdx += incr;
    	    if (voices[i].sineIdx > 1.0f) voices[i].sineIdx -= 1.0f;
    	}
	}
}

void InitVoices(void)
{
	voices[0] = 
	(Voice)
	{
		.baseFrequency = 220.0f,
		.sineIdx = 0.0f
	};
	voices[1] = 
	(Voice)
	{
		.baseFrequency = 440.0f,
		.sineIdx = 0.0f
	};
	voices[2] = 
	(Voice)
	{
		.baseFrequency = 880.0f,
		.sineIdx = 0.0f
	};
	
}



int main()
{
	InitWindow(WIDTH, HEIGHT, "SScape v0.0.1");
	InitAudioDevice();
	SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);
	AudioStream stream = LoadAudioStream(44100, 16, 1);
	SetAudioStreamCallback(stream, AudioInputCallback);

	InitVoices();
	PlayAudioStream(stream);
	SetTargetFPS(60);
	float sliders[MAX_VOICES] = {0};
	while(!WindowShouldClose())
	{
		for(size_t i = 0; i < MAX_VOICES; i++)
		{

			// voices[i].oldFrequency = voices[i].frequency;
			// voices[i].frequency = (sliders[i]/255.0f)*voices[i].baseFrequency;
			voices[i].idx = (int)(sliders[i]/255.0f*11);
		}
		BeginDrawing();
		ClearBackground((Color){(unsigned char)sliders[0], (unsigned char)sliders[1], (unsigned char)sliders[2], 255});
		GuiSlider((Rectangle) {20, HEIGHT - 20, WIDTH - 40, 20}, "0", "255", &sliders[0], 0, 255); 
		GuiSlider((Rectangle) {20, HEIGHT - 40, WIDTH - 40, 20}, "0", "255", &sliders[1], 0, 255);
		GuiSlider((Rectangle) {20, HEIGHT - 60, WIDTH - 40, 20}, "0", "255", &sliders[2], 0, 255);
		DrawText(notes[voices[0].idx], 20, HEIGHT - 90, 20, WHITE);
		DrawText(notes[voices[1].idx], 50, HEIGHT - 90, 20, WHITE);
		DrawText(notes[voices[2].idx], 80, HEIGHT - 90, 20, WHITE);
		
		EndDrawing();
	}
	CloseWindow();
	return 0;
}