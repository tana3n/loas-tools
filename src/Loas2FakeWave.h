#pragma once

int Header = 56;
struct _riffchunk {
	uint32_t chunkId;// RIFF
	uint32_t chunkSize;// WAVE以降の総ファイルサイズ(=合計値-8b)
};
struct _wavechunk {
	uint32_t chunkId;// WAVE Headerchunk
	uint32_t fmt; // "fmt "
	uint32_t chunkSize; //(Option)
	uint16_t bitdepth; 
	uint16_t formatTag; //PCM == 1
	uint16_t channel; // Channel
	uint32_t samplingRate;
	uint32_t byteRate ; // SamplingRate * ch * (Bit/8) = bytes/s
	uint16_t blockSize; // ch * (Bit/8)
};

struct _datachunk {
	uint32_t chunkId;// data
	uint32_t chunkSize;// -126b 
};

struct _waveheader {
	_riffchunk riff;// RIFF
	_wavechunk wave;// WAVE
	_datachunk data;// data
};
