#include "m_transform.h"

static Uint32 * buffer = NULL;
static int buffer_h = 0;
static int buffer_w = 0;

/**
 *
 * Colors the (x,y) pixel with the specific color
 *
 * @param surface target surface
 * @param color color
 * @param x coord
 * @param y coord
 * @return 1 if success, 0 if failed
 */

static short setPixel(SDL_Surface *surface, SDL_Color color, size_t x, size_t y){
	if(surface == NULL){
		fprintf(stderr, "setpixel error");
		return 0;
	}
	Uint32 *pixels=surface->pixels;
	if(pixels == NULL){
		fprintf(stderr, "setpixel error");
		return 0;
	}
	Uint32 new_color=SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
	pixels[y * surface->w + x]=new_color;
	return 1;
}

/**
 * Applies vertical or horizontal symmetry on a surface
 *
 * @param target target image
 * @param vertical 1 for vertical symmetry, 0 for horizontal symmetry
 * @return 1 if success, 0 if failed
 */

short symmetry(image *target, short vertical){
	if(target == NULL){
		fprintf(stderr, "Null argument in symmetry.");
		return 0;
	}
	SDL_Surface *img=get_img_surface(target);
	if(img == NULL){
		fprintf(stderr, "Null surface in symmetry.");
		return 0;
	}
	if(vertical != 0 && vertical != 1){
		fprintf(stderr, "Wrong [vertical] argument in symmetry.");
		return 0;
	}
	SDL_Surface *new_surface;
	new_surface=SDL_CreateRGBSurfaceWithFormat(0, img->w, img->h, 32, img->format->format);
	if(new_surface == NULL){
		SDL_Log("SDL_CreateRGBSurfaceWithFormat() failed: %s", SDL_GetError());
		exit(1);
	}
	if(SDL_MUSTLOCK(new_surface) == SDL_TRUE) SDL_LockSurface(new_surface);
	if(SDL_MUSTLOCK(img) == SDL_TRUE) SDL_LockSurface(img);
	Uint32 *pixels_ref=img->pixels;
	int     height=new_surface->h;
	int     width=new_surface->w;
	// Vertical symmetry
	if(vertical == 1)
		for(int i=0; i < height; i++)
			for(int j=0; j < width; j++){
				SDL_Color c_ref={0};
				SDL_GetRGBA(pixels_ref[i * width + j], new_surface->format, &c_ref.r, &c_ref.g, &c_ref.b, &c_ref.a);
				int rc=setPixel(new_surface, c_ref, i, width - j);
				if(rc == 0){
					SDL_UnlockSurface(new_surface);
					SDL_UnlockSurface(img);
					SDL_FreeSurface(new_surface);
					fprintf(stderr, "pixel not set");
					return 0;
				}
			}
	// Horizontal symmetry
	if(vertical == 0)
		for(int i=0; i < height; i++)
			for(int j=0; j < width; j++){
				SDL_Color c_ref={0};
				SDL_GetRGBA(pixels_ref[i * width + j], new_surface->format, &c_ref.r, &c_ref.g, &c_ref.b, &c_ref.a);
				int rc=setPixel(new_surface, c_ref, height - i, j);
				if(rc == 0){
					SDL_UnlockSurface(new_surface);
					SDL_UnlockSurface(img);
					SDL_FreeSurface(new_surface);
					fprintf(stderr, "pixel not set");
					return 0;
				}
			}
	SDL_UnlockSurface(new_surface);
	SDL_UnlockSurface(img);
	if(set_img_surface(target, new_surface) == 0){
		fprintf(stderr, "Surface not set");
		return 0;
	}
	return 1;
}

/**
 * Rotates the image clockwise
 *
 * @param target image to work with
 * @param angle a positive miltiple of 90,the angle to rotate the surface
 * @return 1 if success, 0 if failed, -1 if surface wasn't created
 */

short rotate(image *target, int angle){
	if(target == NULL){
		fprintf(stderr, "Null image in rotate");
		return 0;
	}
	SDL_Surface *img=get_img_surface(target);
	if(target == NULL){
		fprintf(stderr, "Null surface in rotate");
		return 0;
	}
	// if image is not changed (i.e. 360 degrees)
	if((angle / 90) % 4 == 0)
		return 1;
	// create a surface to be filled
	SDL_Surface *new_surface;
	new_surface=SDL_CreateRGBSurfaceWithFormat(0, img->h, img->w, 32, img->format->format);
	if(new_surface == NULL){
		SDL_Log("SDL_CreateRGBSurfaceWithFormat() failed: %s", SDL_GetError());
		return -1;
	}
	if(SDL_MUSTLOCK(new_surface) == SDL_TRUE) SDL_LockSurface(new_surface);
	if(SDL_MUSTLOCK(img) == SDL_TRUE) SDL_LockSurface(img);
	Uint32 *pixels_ref=img->pixels;
	// when image is turned once clockwise
	if((angle / 90) % 4 == 1){
		int width=new_surface->w;
		for(int i=0; i < img->h; i++)
			for(int j=0; j < img->w; j++){
				SDL_Color c_ref={0};
				SDL_GetRGBA(pixels_ref[i * img->w + j], new_surface->format, &c_ref.r, &c_ref.g, &c_ref.b, &c_ref.a);
				int rc=setPixel(new_surface, c_ref, j, width - i);
				if(rc == 0){
					SDL_UnlockSurface(new_surface);
					SDL_UnlockSurface(img);
					SDL_FreeSurface(new_surface);
					fprintf(stderr, "pixel not set");
					return 0;
				}
			}
	}
	// when image is turned twice clockwise
	if((angle / 90) % 4 == 2){
		int height=new_surface->h;
		int width=new_surface->w;
		for(int i=0; i < img->h; i++)
			for(int j=0; j < img->w; j++){
				SDL_Color c_ref={0};
				SDL_GetRGBA(pixels_ref[i * img->w + j], new_surface->format, &c_ref.r, &c_ref.g, &c_ref.b, &c_ref.a);
				int rc=setPixel(new_surface, c_ref, height - i, width - j);
				if(rc == 0){
					SDL_UnlockSurface(new_surface);
					SDL_UnlockSurface(img);
					SDL_FreeSurface(new_surface);
					fprintf(stderr, "pixel not set");
					return 0;
				}
			}
	}
	// when image is turned three times clockwise
	if((angle / 90) % 4 == 3){
		int height=new_surface->h;
		for(int i=0; i < img->h; i++)
			for(int j=0; j < img->w; j++){
				SDL_Color c_ref={0};
				SDL_GetRGBA(pixels_ref[i * img->w + j], new_surface->format, &c_ref.r, &c_ref.g, &c_ref.b, &c_ref.a);
				int rc=setPixel(new_surface, c_ref, height - j, i);
				if(rc == 0){
					SDL_UnlockSurface(new_surface);
					SDL_UnlockSurface(img);
					SDL_FreeSurface(new_surface);
					fprintf(stderr, "pixel not set");
					return 0;
				}
			}
	}
	SDL_UnlockSurface(new_surface);
	SDL_UnlockSurface(img);
	if(set_img_surface(target, new_surface) == 0){
		fprintf(stderr, "Surface not set");
		return 0;
	}
	return 1;
}


/**
 * @brief
 * Free the buffer that stores copy
 */
static void free_buffer() {
  if(buffer != NULL) {
    Uint32 * tmp = buffer;
    buffer = NULL;
    free(tmp);
  }
}

/**
 * @brief
 * Tell if the pixel is inside the image
 */
static short is_in_image(int x, int y, int w, int h) {
 return (0 < y) && (y < h) && (0 < x) && (x < w); 
}

/*
 * @brief
 * Copy an area into the buffer
 *
 * @param img image on which you have to copy
 * @param SDL_Rect area where you want to copy
 * @return 0 in case of error 1 else
 */
short copy(image *img, SDL_Rect area) {
  if(img == NULL){
		fprintf(stderr, "Null image in copy\n");
		return 0;
	}
	SDL_Surface *current = get_img_surface(img);
	if(current == NULL){
		fprintf(stderr, "Null surface in copy\n");
		return 0;
	}
  free_buffer();
  buffer_h = area.h;
  buffer_w = area.w;
  buffer = malloc(area.w * area.h * sizeof(Uint32));
  if (buffer == NULL) {
    fprintf(stderr, "Buffer copy error\n");
    return 0;
  }
  if(SDL_MUSTLOCK(current) == SDL_TRUE) SDL_LockSurface(current);
	Uint32 *pixels = current->pixels;
	
  for(int i = 0 ; i < area.h ; i++) {
    for (int j = 0 ; j < area.w ; j++) {
      buffer[i*area.w+j] = pixels[(area.y+i)*area.w + (area.x+j)]; 
    }
  }
  SDL_UnlockSurface(current);
  return 1;
}

/*
 * @brief
 * Cut an area into the buffer
 *
 * @param img image on which you have to copy
 * @param SDL_Rect area where you want to copy
 * @return 0 in case of error 1 else
 */
short cut(image *img, SDL_Rect area) {
  if(img == NULL){
		fprintf(stderr, "Null image in cut\n");
		return 0;
	}
	SDL_Surface *current = get_img_surface(img);
	if(current == NULL){
		fprintf(stderr, "Null surface in cut\n");
		return 0;
	}
  free_buffer();
  
  buffer_h = area.h; 
  buffer_w = area.w;
  buffer = malloc(area.w * area.h * sizeof(Uint32));
  if (buffer == NULL) {
    fprintf(stderr, "Buffer cut error\n");
    return 0;
  }
  if(SDL_MUSTLOCK(current) == SDL_TRUE) SDL_LockSurface(current);
	Uint32 *pixels = current->pixels;
	
  for(int i = 0 ; i < area.h ; i++) {
    for (int j = 0 ; j < area.w ; j++) {
      buffer[i*area.w+j] = pixels[(area.y+i)*area.w + (area.x+j)]; 
      pixels[(area.y+i)*area.w + (area.x+j)] = 0; // Couleur noire 
    }
  }
  SDL_UnlockSurface(current);
  return 1;
}


/**
 * @brief
 * Paste buffer onto img->surface
 * @param img image to paste onto
 * @param x y coordinates
 */
short paste(image *img, int x, int y) {
  if (buffer == NULL) return 1;
   if(img == NULL){
		fprintf(stderr, "Null image in paste\n");
		return 0;
	}
	SDL_Surface *current = get_img_surface(img);
	if(current == NULL){
		fprintf(stderr, "Null surface in paste\n");
		return 0;
	}
  if(SDL_MUSTLOCK(current) == SDL_TRUE) SDL_LockSurface(current);
	Uint32 *pixels = current->pixels;
  for(int i = 0 ; i < buffer_h ; i++) {
    for (int j = 0 ; j < buffer_w ; j++) {
      if (is_in_image(x+j, y+i ,current->w,current->h)) {
          pixels[(y+i)*buffer_w + (x+j)] = buffer[i*buffer_w+j];
      }
    }
  }
  SDL_UnlockSurface(current);
  return 1;
}
