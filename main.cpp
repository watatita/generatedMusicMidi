#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NOTE_C4  60
#define NOTE_C4shp  61
#define NOTE_D4  62
#define NOTE_D4shp  63
#define NOTE_E4  64
#define NOTE_F4  65
#define NOTE_F4shp  66
#define NOTE_G4  67
#define NOTE_G4shp  68
#define NOTE_A4  69
#define NOTE_A4shp  70
#define NOTE_B4  71
#define NOTE_C5  72


using namespace std;

union VI32
{
    int ii;
    unsigned char b[4];
}var32;

union VI16
{
    short ii;
    unsigned char b[2];
}var16;

unsigned char* fileStream;
int filePosition=0;
int trStartPos;
int trackStartPos[32];

//void gmWriteMidiHeader(FILE* ff);
//void gmWriteTrackHeader(FILE* ff);
//void gmWriteTimeSignature(FILE* ff,int tsNumerator,int tsDenumerator,
//                          int metronomeClick=24,int numberOfNotes=8);
//
//void gmWriteSound(FILE* ff,int channel,int pitch,int velocity,int lenght);
//void gmWriteSystem(FILE* ff);
//void gmWriteEndOfTrack(FILE* ff);

void gmBufferMidiHeader(unsigned char* ff,int midiformat,int miditracks,int mididiv);
void gmBufferTrackHeader(unsigned char* ff);

//ff 02
void gmBufferCopyright(unsigned char* ff,const char* copyrightText);
//ff 03
void gmBufferTitleName(unsigned char* ff,const char* titleName);
//ff 2f
void gmBufferEndOfTrack(unsigned char* ff);
//ff 51
void gmBufferTempo(unsigned char* ff,int tempo);
//ff 58
void gmBufferTimeSignature(unsigned char* ff,int tsNumerator,int tsDenumerator,int metronomeClick=24,int numberOfNotes=8);
//ff 59
void gmBufferKeySignature(unsigned char* ff,int sharpflat,bool majorminor);

void gmBufferMusicNote(unsigned char* ff,int channel,int pitch,int velocity,int lenght);
void gmBufferSystem(unsigned char* ff);

void gmFinalize(FILE* gmfile,unsigned char* ff);

int main()
{
    FILE* fileMidi=fopen("test.mid","wb");
    fileStream=(unsigned char*) malloc(0x3ffff);

    gmBufferMidiHeader(fileStream,1,1,480);
    gmBufferTrackHeader(fileStream);
//    gmBufferCopyright(fileStream,"(C) Watatita micro_field");
    gmBufferTimeSignature(fileStream,4,2);
    gmBufferKeySignature(fileStream,1,0);
    gmBufferMusicNote(fileStream,0,NOTE_C4,80,480);
    gmBufferMusicNote(fileStream,0,NOTE_D4,80,480);
    gmBufferMusicNote(fileStream,0,NOTE_E4,80,480);
    gmBufferMusicNote(fileStream,0,NOTE_D4,80,480);
    gmBufferMusicNote(fileStream,0,NOTE_C4,80,960);
    gmBufferEndOfTrack(fileStream);
    gmFinalize(fileMidi,fileStream);

    fclose(fileMidi);
    return 0;
}

void gmBufferMidiHeader(unsigned char* ff,int midiformat,int miditracks,int mididiv)
{
    ff[filePosition]='M'; filePosition++;
    ff[filePosition]='T'; filePosition++;
    ff[filePosition]='h'; filePosition++;
    ff[filePosition]='d'; filePosition++;

    ff[filePosition]=0x00; filePosition++;
    ff[filePosition]=0x00; filePosition++;
    ff[filePosition]=0x00; filePosition++;
    ff[filePosition]=0x06; filePosition++;

    ff[filePosition]=0x00; filePosition++;
    ff[filePosition]=midiformat; filePosition++;

    var16.ii=miditracks;
    ff[filePosition]=var16.b[1]; filePosition++;
    ff[filePosition]=var16.b[0]; filePosition++;

    var16.ii=mididiv;
    ff[filePosition]=var16.b[1]; filePosition++;
    ff[filePosition]=var16.b[0]; filePosition++;
}

void gmBufferTrackHeader(unsigned char* ff)
{
    ff[filePosition]='M'; filePosition++;
    ff[filePosition]='T'; filePosition++;
    ff[filePosition]='r'; filePosition++;
    ff[filePosition]='k'; filePosition++;
    //temporary lenght
    ff[filePosition]=0; filePosition++;
    ff[filePosition]=0; filePosition++;
    ff[filePosition]=0; filePosition++;
    ff[filePosition]=0; filePosition++;
    trStartPos=filePosition;
    //first delta time
    ff[filePosition]=0; filePosition++;
}

//ff 02
void gmBufferCopyright(unsigned char* ff,const char* copyrightText)
{
    int charlen=strlen(copyrightText);
    int i;

    ff[filePosition]=0xff; filePosition++;
    ff[filePosition]=0x02; filePosition++;
    ff[filePosition]=charlen; filePosition++;
    for(i=0;i<charlen;i++)
    {
        ff[filePosition]=copyrightText[i]; filePosition++;
    }
    //delta time
    ff[filePosition]=0; filePosition++;
}
//ff 03
void gmBufferTitleName(unsigned char* ff,const char* titleName)
{
    int charlen=strlen(titleName);
    int i;

    ff[filePosition]=0xff; filePosition++;
    ff[filePosition]=0x03; filePosition++;
    ff[filePosition]=charlen; filePosition++;
    for(i=0;i<charlen;i++)
    {
        ff[filePosition]=titleName[i]; filePosition++;
    }
    //delta time
    ff[filePosition]=0; filePosition++;
}
//ff 2f
void gmBufferEndOfTrack(unsigned char* ff)
{
    ff[filePosition]=0xff; filePosition++;
    ff[filePosition]=0x2f; filePosition++;
    ff[filePosition]=0; filePosition++;
}
//ff 51
void gmBufferTempo(unsigned char* ff,int tempo=500000)
{
    ff[filePosition]=0xff; filePosition++;
    ff[filePosition]=0x51; filePosition++;
    ff[filePosition]=0x03; filePosition++;

    var32.ii=tempo;
    ff[filePosition]=var32.b[2]; filePosition++;
    ff[filePosition]=var32.b[1]; filePosition++;
    ff[filePosition]=var32.b[0]; filePosition++;

    //delta time
    ff[filePosition]=0; filePosition++;
}
//ff 58
void gmBufferTimeSignature(unsigned char* ff,int tsNumerator,int tsDenumerator,int metronomeClick,int numberOfNotes)
{
    ff[filePosition]=0xff; filePosition++;
    ff[filePosition]=0x58; filePosition++;
    ff[filePosition]=0x04; filePosition++;

    ff[filePosition]=tsNumerator;       filePosition++;
    ff[filePosition]=tsDenumerator;     filePosition++;
    ff[filePosition]=metronomeClick;    filePosition++;
    ff[filePosition]=numberOfNotes;     filePosition++;

    //delta time
    ff[filePosition]=0; filePosition++;
}
//ff 59
void gmBufferKeySignature(unsigned char* ff,int sharpflat,bool majorminor)
{
    ff[filePosition]=0xff; filePosition++;
    ff[filePosition]=0x59; filePosition++;
    ff[filePosition]=0x02; filePosition++;

    ff[filePosition]=sharpflat;  filePosition++;
    ff[filePosition]=majorminor; filePosition++;

    //delta time
    ff[filePosition]=0; filePosition++;
}

void gmBufferMusicNote(unsigned char* ff,int channel,int pitch,int velocity,int lenght)
{
    channel=channel&0x0f;
    ff[filePosition]=0x90|channel;  filePosition++;
    ff[filePosition]=pitch;         filePosition++;
    ff[filePosition]=velocity;      filePosition++;

    int tByte0= lenght      & 0x7f;
    int tByte1=(lenght>> 7) & 0x7f;
    int tByte2=(lenght>>14) & 0x7f;
    int tByte3=(lenght>>21) & 0x7f;

    //delta time
    if(tByte3)
    {
        ff[filePosition]=tByte3|0x80; filePosition++;
        ff[filePosition]=tByte2|0x80; filePosition++;
        ff[filePosition]=tByte1|0x80; filePosition++;
        ff[filePosition]=tByte0;      filePosition++;
    }else if(tByte2)
    {
        ff[filePosition]=tByte2|0x80; filePosition++;
        ff[filePosition]=tByte1|0x80; filePosition++;
        ff[filePosition]=tByte0;      filePosition++;
    }else if(tByte1)
    {
        ff[filePosition]=tByte1|0x80; filePosition++;
        ff[filePosition]=tByte0;      filePosition++;
    }else
    {
        ff[filePosition]=tByte0;      filePosition++;
    }

    //off
    ff[filePosition]=0x80|channel;  filePosition++;
    ff[filePosition]=pitch;         filePosition++;
    ff[filePosition]=velocity;      filePosition++;
    //delta time
    ff[filePosition]=0;             filePosition++;

}

void gmFinalize(FILE* gmfile,unsigned char* ff)
{
    var32.ii=filePosition-trStartPos;
    printf("%d",var32.ii);
    ff[trStartPos-4]=var32.b[3];
    ff[trStartPos-3]=var32.b[2];
    ff[trStartPos-2]=var32.b[1];
    ff[trStartPos-1]=var32.b[0];

    int i;
    for(i=0;i<filePosition;i++)
    {
        fputc(ff[i],gmfile);
    }
}

/*
    fputc('M',fileMidi);
    fputc('T',fileMidi);
    fputc('h',fileMidi);
    fputc('d',fileMidi);
    var32.ii=6;
    for(int i=0;i<4;i++)
    {
        fputc(var32.b[3-i],fileMidi);
    }

    fputc(0,fileMidi);
    fputc(0,fileMidi);

    fputc(0,fileMidi);
    fputc(1,fileMidi);

    fputc(0x01,fileMidi);
    fputc(0xe0,fileMidi);

    fputc('M',fileMidi);
    fputc('T',fileMidi);
    fputc('r',fileMidi);
    fputc('k',fileMidi);

    //length
    var32.ii=15;
    fputc(var32.b[3],fileMidi);
    fputc(var32.b[2],fileMidi);
    fputc(var32.b[1],fileMidi);
    fputc(var32.b[0],fileMidi);
    //
    fputc(0,fileMidi);//time
    fputc(0xff,fileMidi);
    fputc(0x59,fileMidi);
    fputc(0x02,fileMidi);
    fputc(0x01,fileMidi);
    fputc(0x00,fileMidi);
    fputc(0,fileMidi);//time
    fputc(0x90,fileMidi);//on channel 0
    fputc(0x3c,fileMidi);//c4
    fputc(0x50,fileMidi);
    fputc(0x83,fileMidi);//time
    fputc(0x60,fileMidi);//time
    fputc(0x80,fileMidi);//off channel 0
    fputc(0x3c,fileMidi);
    fputc(0x00,fileMidi);
    fputc(0,fileMidi);//time
    */
