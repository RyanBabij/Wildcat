/*
LodePNG Examples

Copyright (c) 2005-2008 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

/*These examples demonstrate the usage of LodePNG in C and are useful as test.
Place the example code in a new .c file, and compile together with lodepng.c
and lodepng.h, with optimization.
Run the example executables from command line with the filename as parameter.*/

/*gcc -lm lodepng.c lodepng_examples.c -ansi -pedantic -Wall -Wextra -lSDL -O3*/


/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG console example                                                    */
/* This example decodes a PNG file and displays its size in the console       */
/* ////////////////////////////////////////////////////////////////////////// */

#include "lodepng.h"

void example1(int argc, char *argv[])
{
  const char* filename = argc > 1 ? argv[1] : "test.png";

  unsigned char* buffer;
  unsigned char* image;
  size_t buffersize, imagesize, i;
  LodePNG_Decoder decoder;
  
  LodePNG_loadFile(&buffer, &buffersize, filename); /*load the image file with given filename*/
  LodePNG_Decoder_init(&decoder);
  LodePNG_decode(&decoder, &image, &imagesize, buffer, buffersize); /*decode the png*/
  
  /*load and decode*/
  /*if there's an error, display it, otherwise display information about the image*/
  if(decoder.error) printf("error: %d\n", decoder.error);
  else
  {
    printf("w: %d\n", decoder.infoPng.width);
    printf("h: %d\n", decoder.infoPng.height);
    printf("bitDepth: %d\n", decoder.infoPng.color.bitDepth);
    printf("bpp: %d\n", LodePNG_InfoColor_getBpp(&decoder.infoPng.color));
    printf("colorChannels: %d\n", LodePNG_InfoColor_getChannels(&decoder.infoPng.color));
    printf("paletteSize: %d\n", decoder.infoPng.color.palettesize);
    printf("colorType: %d\n", decoder.infoPng.color.colorType);
    printf("compressionMethod: %d\n", decoder.infoPng.compressionMethod);
    printf("filterMethod: %d\n", decoder.infoPng.filterMethod);
    printf("interlaceMethod: %d\n", decoder.infoPng.interlaceMethod);
    for(i = 0; i < decoder.infoPng.text.num; i++)
      printf("%s: %s\n",  decoder.infoPng.text.keys[i], decoder.infoPng.text.strings[i]);
    for(i = 0; i < decoder.infoPng.itext.num; i++)
      printf("%s (%s %s) : %s\n",  decoder.infoPng.itext.keys[i], decoder.infoPng.itext.langtags[i], decoder.infoPng.itext.transkeys[i], decoder.infoPng.itext.strings[i]);
    if(decoder.infoPng.time_defined)
    {
      printf("modification time: %d-%d-%d %d:%d:%d\n", decoder.infoPng.time.year, decoder.infoPng.time.month, 
             decoder.infoPng.time.day, decoder.infoPng.time.hour, decoder.infoPng.time.minute, decoder.infoPng.time.second);
    }
    if(decoder.infoPng.phys_defined)
    {
      printf("physical size: %d %d %d\n", decoder.infoPng.phys_x, decoder.infoPng.phys_y, (int)decoder.infoPng.phys_unit);
    }
  }
  
  /*cleanup decoder*/
  free(image);
  free(buffer);
  LodePNG_Decoder_cleanup(&decoder);
}

/*#define ENABLE_SDL_EXAMPLE*/
#ifdef ENABLE_SDL_EXAMPLE

/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG SDL example                                                        */
/* This example displays a PNG with a checkerboard pattern to show tranparency*/
/* It requires the SDL library to compile and run                             */
/* g++ *.cpp -lSDL -O3                                                        */
/* ////////////////////////////////////////////////////////////////////////// */

#include <SDL/SDL.h> /*requires SDL*/
#include "lodepng.h"

void example2(int argc, char *argv[])
{
  const char* filename = argc > 1 ? argv[1] : "test.png";
  
  unsigned error;
  unsigned char* image;
  size_t w, h, x, y;
  
  /*load the PNG in one function call*/
  error = LodePNG_decode32f(&image, &w, &h, filename);

  /*stop if there is an error*/
  if(error)
  {
    printf("error: %d\n", error);
    return;
  }
  
  while(!error) /*simply used to avoid gotos when an error happens, we never really loop*/
  {
    SDL_Surface* scr;
    SDL_Event event;
    int done;
    
    /*avoid too large window size by downscaling large image*/
    size_t jump = 1;
    if(w / 1024 >= jump) jump = w / 1024 + 1;
    if(h / 1024 >= jump) jump = h / 1024 + 1;
    
    /*init SDL*/
    if(SDL_Init(SDL_INIT_VIDEO) < 0) break;
    scr = SDL_SetVideoMode(w / jump, h / jump, 32, SDL_HWSURFACE);
    if(!scr) break;
    SDL_WM_SetCaption(filename, NULL); /*set window caption*/
    
    /*plot the pixels of the PNG file*/
    for(y = 0; y + jump - 1 < h; y += jump)
    for(x = 0; x + jump - 1 < w; x += jump)
    {
      int checkerColor;
      Uint32* bufp;
      Uint32 r, g, b, a;
      
      /*get RGBA components*/
      r = image[4 * y * w + 4 * x + 0]; /*red*/
      g = image[4 * y * w + 4 * x + 1]; /*green*/
      b = image[4 * y * w + 4 * x + 2]; /*blue*/
      a = image[4 * y * w + 4 * x + 3]; /*alpha*/
      
      /*make translucency visible by placing checkerboard pattern behind image*/
      checkerColor = 191 + 64 * (((x / 16) % 2) == ((y / 16) % 2));
      r = (a * r + (255 - a) * checkerColor) / 255;
      g = (a * g + (255 - a) * checkerColor) / 255;
      b = (a * b + (255 - a) * checkerColor) / 255;
      
      /*give the color value to the pixel of the screenbuffer*/
      bufp = (Uint32 *)scr->pixels + (y * scr->pitch / 4) / jump + (x / jump);
      *bufp = 65536 * r + 256 * g + b;
    }
    
    /*pause until you press escape and meanwhile redraw screen*/
    done = 0;
    while(done == 0)
    {
      while(SDL_PollEvent(&event))
      {
        if(event.type == SDL_QUIT) done = 1;
        if(SDL_GetKeyState(NULL)[SDLK_ESCAPE]) done = 1;
      }
      SDL_UpdateRect(scr, 0, 0, 0, 0); /*redraw screen*/
      SDL_Delay(5); /*pause 5 ms so it consumes less processing power*/
    }
    
    break;
  }
  
  /*cleanup*/
  free(image);
  SDL_Quit();
}

#endif


/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG Encoder example                                                    */
/* This example encodes a PNG containing a generated texture                  */
/* ////////////////////////////////////////////////////////////////////////// */

#include <math.h>
#include "lodepng.h"

void example3(int argc, char *argv[])
{
  unsigned char* buffer;
  size_t buffersize;
  LodePNG_Encoder encoder;
  const int w = 512;
  const int h = 512;
  unsigned char* image;
  int x, y;
  
  /*check if user gave a filename*/
  if(argc < 2)
  {
    printf("please provide a filename to save to\n");
    return;
  }
  
  /*generate some image*/
  image = (unsigned char*)malloc(w * h * 4);
  for(y = 0; y < h; y++)
  for(x = 0; x < w; x++)
  {
    /*pattern 1*/
    image[4 * w * y + 4 * x + 0] = (unsigned char)(127 * (1 + sin((                    x * x +                     y * y) / (w * h / 8.0))));
    image[4 * w * y + 4 * x + 1] = (unsigned char)(127 * (1 + sin(((w - x - 1) * (w - x - 1) +                     y * y) / (w * h / 8.0))));
    image[4 * w * y + 4 * x + 2] = (unsigned char)(127 * (1 + sin((                    x * x + (h - y - 1) * (h - y - 1)) / (w * h / 8.0))));
    image[4 * w * y + 4 * x + 3] = (unsigned char)(127 * (1 + sin(((w - x - 1) * (w - x - 1) + (h - y - 1) * (h - y - 1)) / (w * h / 8.0))));
    
    /*pattern 2*/
    /*image[4 * w * y + 4 * x + 0] = 255 * !(x & y);
    image[4 * w * y + 4 * x + 1] = x ^ y;
    image[4 * w * y + 4 * x + 2] = x | y;
    image[4 * w * y + 4 * x + 3] = 255;*/
  }
  
  /*create encoder and set settings and info (optional)*/
  LodePNG_Encoder_init(&encoder);
  encoder.settings.zlibsettings.windowSize = 2048;
  LodePNG_Text_add(&encoder.infoPng.text, "Comment", "Created with LodePNG");

  /*encode and save*/
  LodePNG_encode(&encoder, &buffer, &buffersize, image, w, h);
  LodePNG_saveFile(buffer, buffersize, argv[1]);
  
  /*cleanup*/
  LodePNG_Encoder_cleanup(&encoder);
  free(buffer);
  free(image);
}


/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG greyscale example                                                  */
/* Load a PNG directly as greyscale if it's greyscale, convert otherwise      */
/* ////////////////////////////////////////////////////////////////////////// */

#include "lodepng.h"

void example4(int argc, char *argv[])
{
  const char* filename = argc > 1 ? argv[1] : "test.png";
  unsigned char* grey; /*in here the greyscale image will be stored*/
  size_t i, greysize;
  unsigned char* buffer; /*the file*/
  size_t buffersize;
  LodePNG_Decoder decoder;
  
  /*load and decode*/
  LodePNG_loadFile(&buffer, &buffersize, filename); /*load the image file with given filename*/
  LodePNG_Decoder_init(&decoder);
  LodePNG_inspect(&decoder, buffer, buffersize); /*get information from the header, to see if it's greyscale*/
  
  if(!decoder.error)
  {
    if(LodePNG_InfoColor_isGreyscaleType(&decoder.infoPng.color)) /*only if the PNG image is greyscale, can the output be converted to greyscale by LodePNG*/
    {
      printf("it's a greyscale PNG\n");
      decoder.infoRaw.color.colorType = 0; /*set color type "greyscale" for the output*/
      LodePNG_decode(&decoder, &grey, &greysize, buffer, buffersize);
    }
    else /*else you have to do color math to convert to greyscale*/
    {
      unsigned char* rgb; /*temporary rgb image*/
      size_t rgbsize;
      
      printf("it's a color PNG\n");
      decoder.infoRaw.color.colorType = 2; /*set color type "RGB" for the output*/
      LodePNG_decode(&decoder, &rgb, &rgbsize, buffer, buffersize);
      
      greysize = rgbsize / 3;
      grey = (unsigned char*)malloc(greysize);
      for(i = 0; i < greysize; i++) grey[i] = (rgb[i * 3 + 0] + rgb[i * 3 + 1] + rgb[i * 3 + 2]) / 3; /*rgb to greyscale by taking average*/
      
      free(rgb);
    }
  }
  
  if(decoder.error) printf("error: %d\n", decoder.error);
  else printf("read %d greyscale pixels\n", greysize);
  
  /*cleanup decoder*/
  free(grey);
  free(buffer);
  LodePNG_Decoder_cleanup(&decoder);
}


/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG palette example                                                    */
/* This example encodes a PNG with a palette                                  */
/* Both image and palette contain sine waves, resulting in a sort of plasma   */
/* ////////////////////////////////////////////////////////////////////////// */

#include <math.h>
#include "lodepng.h"

void example5(int argc, char *argv[])
{
  
  /*create encoder and set settings and info (optional)*/
  LodePNG_Encoder encoder;
  size_t x, y, i;
  unsigned char* image;
  unsigned char* buffer;
  size_t buffersize;
  const unsigned w = 512;
  const unsigned h = 512;

  /*check if user gave a filename*/
  if(argc < 2)
  {
    printf("please provide a filename to save to\n");
    return;
  }
  
  LodePNG_Encoder_init(&encoder);
  
  LodePNG_Text_add(&encoder.infoPng.text, "Comment", "Created with LodePNG");
  
  /*Generate palette. Note how only the palette of infoPng has to be set, not that of infoRaw. That is because
  we'll encode the PNG as a palettized PNG and the palette stored in LodePNG_InfoPng is used for the PLTE and tRNS
  chunk of the PNG. When encoding a palettized raw image to a PNG without palette (RGB, RGBA, ...), you'd
  have to set the palette of infoRaw instead of that of infoPng, because then the palette info of infoRaw is
  used by the encoder to do the conversion to RGBA.*/
  for(i = 0; i < 256; i++)
  {
    LodePNG_InfoColor_addPalette(&encoder.infoPng.color
                        , (unsigned char)(127 * (1 + sin(5 * i * 6.28318531 / 256)))
                        , (unsigned char)(127 * (1 + sin(2 * i * 6.28318531 / 256)))
                        , (unsigned char)(127 * (1 + sin(3 * i * 6.28318531 / 256)))
                        , (unsigned char)( 63 * (1 + sin(8 * i * 6.28318531 / 256))) + 128); /*alpha channel of the palette (tRNS chunk)*/
  }
  
  /*both the raw image and the encoded image must get colorType 3 (palette)*/
  encoder.infoPng.color.colorType = 3; /*if you comment this line, and store the palette in LodePNG_InfoRaw instead (use getLodePNG_InfoRaw() in the previous lines), then you get the same image in a RGBA PNG.*/
  encoder.infoRaw.color.colorType = 3;
  
  /*generate some image*/
  image = (unsigned char*)malloc(w * h);
  for(y = 0; y < h; y++)
  for(x = 0; x < w; x++)
  {
    image[w * y + x] = (unsigned char)(63 * ((1 + sin((2 * 6.28318531) * (x / (float)w))) + (1 + sin((2 * 6.28318531) * (y / (float)h)))));
  }
  
  /*encode and save*/
  LodePNG_encode(&encoder, &buffer, &buffersize, image, w, h);
  if(encoder.error) printf("Encoder error: %d\n", encoder.error);
  LodePNG_saveFile(buffer, buffersize, argv[1]);
  
  /*cleanup*/
  free(image);
  free(buffer);
  LodePNG_Encoder_cleanup(&encoder);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG Ghostifier                                                         */
/* Loads a PNG, modifies its alpha channel to a ghost effect, saves the result*/
/* The result is visible in browsers that support PNG transparency            */
/* ////////////////////////////////////////////////////////////////////////// */

#include "lodepng.h"

void example6(int argc, char *argv[])
{
  unsigned char* image;
  unsigned char* buffer;
  size_t buffersize;
  unsigned w, h;
  unsigned error;
  
  /*check if user gave a filename*/
  if(argc < 2)
  {
    printf("please provide the name of a file to ghostify\n");
    return;
  }
  
  LodePNG_loadFile(&buffer, &buffersize, argv[1]);
  
  error = LodePNG_decode32(&image, &w, &h, buffer, buffersize);
  if(!error)
  {
    size_t x, y;
    /*ghostify image*/
    for(y = 0; y < h; y++)
    for(x = 0; x < w; x++)
    {
      size_t index = 4 * w * y + 4 * x;
      image[index + 3] = (unsigned char)(((int)(image[index + 0]) + (int)(image[index + 1]) + (int)(image[index + 2])) / 3);
    }
    
    /*encode and save*/
    free(buffer);
    buffer = 0;
    buffersize = 0;
    LodePNG_encode32(&buffer, &buffersize, image, w, h);
    
    LodePNG_saveFile(buffer, buffersize, argv[1]);
  }
  
  /*cleanup*/
  free(image);
  free(buffer);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG 4-bit palette example                                              */
/* This example encodes a 511x511 PNG with a 4-bit palette                    */
/* Both image and palette contain sine waves, resulting in a sort of plasma   */
/* ////////////////////////////////////////////////////////////////////////// */

#include <math.h>
#include "lodepng.h"

void example7(int argc, char *argv[])
{
  
  /*create encoder and set settings and info (optional)*/
  LodePNG_Encoder encoder;
  size_t x, y, i;
  unsigned char* image;
  unsigned char* buffer;
  size_t buffersize;
  const unsigned w = 511;
  const unsigned h = 511;

  /*check if user gave a filename*/
  if(argc < 2)
  {
    printf("please provide a filename to save to\n");
    return;
  }
  
  LodePNG_Encoder_init(&encoder);
  
  LodePNG_Text_add(&encoder.infoPng.text, "Comment", "Created with LodePNG");
  
  /*Generate palette. Note how only the palette of infoPng has to be set, not that of infoRaw. That is because
  we'll encode the PNG as a palettized PNG and the palette stored in LodePNG_InfoPng is used for the PLTE and tRNS
  chunk of the PNG. When encoding a palettized raw image to a PNG without palette (RGB, RGBA, ...), you'd
  have to set the palette of infoRaw instead of that of infoPng, because then the palette info of infoRaw is
  used by the encoder to do the conversion to RGBA.
  NOTE: some software can't properly display this 4-bit palette with semi-translucent values.
  */
  for(i = 0; i < 16; i++)
  {
    LodePNG_InfoColor_addPalette(&encoder.infoPng.color
                        , (unsigned char)(127 * (1 + sin(5 * i * 6.28318531 / 16)))
                        , (unsigned char)(127 * (1 + sin(2 * i * 6.28318531 / 16)))
                        , (unsigned char)(127 * (1 + sin(3 * i * 6.28318531 / 16)))
                        , (unsigned char)( 63 * (1 + sin(8 * i * 6.28318531 / 16))) + 128); /*alpha channel of the palette (tRNS chunk)*/
  }
  
  /*both the raw image and the encoded image must get colorType 3 (palette)*/
  encoder.infoPng.color.colorType = 3; /*if you comment this line, and store the palette in LodePNG_InfoRaw instead (use getLodePNG_InfoRaw() in the previous lines), then you get the same image in a RGBA PNG.*/
  encoder.infoRaw.color.colorType = 3;
  encoder.infoPng.color.bitDepth = 4;
  encoder.infoRaw.color.bitDepth = 4;
  encoder.infoPng.interlaceMethod = 1;
  encoder.settings.text_compression = 1;
  
  /*generate some 4-bit image, without padding bits (scanlines don't necessarily start at an exact byte beginning)*/
  image = (unsigned char*)malloc((w * h * 4 + 7) / 8);
  for(y = 0; y < h; y++)
  for(x = 0; x < w; x+=2)
  {
    size_t index = (y * w + x) / 2;
    image[index] = (unsigned char)(63 * ((1 + sin((2 * 6.28318531) * (x / (float)w))) + (1 + sin((2 * 6.28318531) * (y / (float)h)))));
  }
  
  /*encode and save*/
  LodePNG_encode(&encoder, &buffer, &buffersize, image, w, h);
  if(encoder.error) printf("Encoder error: %d\n", encoder.error);
  LodePNG_saveFile(buffer, buffersize, argv[1]);
  
  /*cleanup*/
  free(image);
  free(buffer);
  LodePNG_Encoder_cleanup(&encoder);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG decode-encode: decodes the image, then encodes it again            */
/* colorType, bitDepth and interlace will be the same, no matter what it was  */
/* Basically, all known chunks are interpreted and then regenerated, while all*/
/* unknown chunks are simply placed back literally                            */
/* ////////////////////////////////////////////////////////////////////////// */

#include "lodepng.h"

void example8(int argc, char *argv[])
{
  unsigned char* image = 0;
  unsigned char* buffer = 0;
  size_t imagesize = 0;
  size_t buffersize = 0;
  LodePNG_Encoder encoder;
  LodePNG_Decoder decoder;
  
  /*check if user gave a filename*/
  if(argc < 3)
  {
    printf("please provide out and in filename\n");
    return;
  }
  
  LodePNG_Encoder_init(&encoder);
  LodePNG_Decoder_init(&decoder);
  
  decoder.settings.color_convert = 0;
  decoder.settings.rememberUnknownChunks = 1; /*make it reproduce even unknown chunks in the saved image*/
  
  LodePNG_loadFile(&buffer, &buffersize, argv[2]);
  LodePNG_decode(&decoder, &image, &imagesize, buffer, buffersize);
  if(decoder.error)
  {
    printf("decoder error %d\n", decoder.error);
    goto cleanup; /*C has no automatically invoked destructors so here's a valid use for a goto*/
  }
  
  LodePNG_InfoPng_copy(&encoder.infoPng, &decoder.infoPng);
  LodePNG_InfoRaw_copy(&encoder.infoRaw, &decoder.infoRaw); /*the decoder has written the PNG colortype in the infoRaw too*/
  
  free(buffer);
  buffer = 0;
  buffersize = 0;
  
  LodePNG_encode(&encoder, &buffer, &buffersize, image, decoder.infoPng.width, decoder.infoPng.height);
  if(encoder.error)
  {
    printf("encoder error %d\n", encoder.error);
    goto cleanup; /*C has no automatically invoked destructors so here's a valid use for a goto*/
  }
  
  if(!encoder.error) LodePNG_saveFile(buffer, buffersize, argv[1]);
  
  /*cleanup*/
  cleanup:
  free(image);
  free(buffer);
  LodePNG_Encoder_cleanup(&encoder);
  LodePNG_Decoder_cleanup(&decoder);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG chunk lister: lists the types of all chunks in the PNG image       */
/* ////////////////////////////////////////////////////////////////////////// */

#include "lodepng.h"

void example9(int argc, char *argv[]) /*list the chunks*/
{
  const char* filename = argc > 1 ? argv[1] : "test.png";
  unsigned char *chunk, *begin, *end;
  unsigned char* buffer = 0;
  size_t buffersize = 0;
  
  LodePNG_loadFile(&buffer, &buffersize, filename); /*load the image file with given filename*/
  end = &buffer[buffersize - 1] + 1;
  begin = chunk = &buffer[0] + 8;
  
  printf("type / length\n");
  printf("-------------\n");
  while(chunk + 8 < end && chunk >= begin)
  {
    char type[5];
    LodePNG_chunk_type(type, chunk);
    printf("%s / %d\n", type, LodePNG_chunk_length(chunk));
    chunk = LodePNG_chunk_next(chunk);
  }
}

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */

int main(int argc, char *argv[])
{
  /*NOTE: many of these examples overwrite a file without asking so watch out what you give as parameter to the program!*/
  
  example1(argc, argv); /*console example*/
  /*example2(argc, argv);*/ /*SDL example (enable the #define above)*/
  /*example3(argc, argv);*/ /*encoder example*/
  /*example4(argc, argv);*/ /*greyscale example*/
  /*example5(argc, argv);*/ /*palette example*/
  /*example6(argc, argv);*/ /*ghostify example*/
  /*example7(argc, argv);*/ /*4-bit palette example*/
  /*example8(argc, argv);*/ /*re-encode example*/
  /*example9(argc, argv);*/ /*chunk listing example*/
  
  return 0;
}
