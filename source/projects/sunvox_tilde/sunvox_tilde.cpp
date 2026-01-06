/**
	@file
	sv~: a simple audio object for Max
	original by: jeremy bernstein, jeremy@bootsquad.com
	@ingroup examples
*/

#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects

#include <dlfcn.h>
#include <math.h>
// #include <signal.h>
#include <stdlib.h>
#include <string.h>

#define SUNVOX_STATIC_LIB
#include <sunvox.h>

#define N_IN_CHANNELS 2
#define N_OUT_CHANNELS 2
#define FLOAT32_TYPE 1
#define LATENCY 0

// struct to represent the object's state
typedef struct _sv {
	t_pxobject		ob;	       // the object itself (t_pxobject in MSP instead of t_object)
	int is_initialized;        // flag to indicate if sv_init has been successfully called
    const char* resources_dir; // resource directory inside external bundle
	float *in_sv_buffer;       // intermediate sunvox input buffer
    float *out_sv_buffer;      // intermediate sunvox output buffer
} t_sv;


// method prototypes
void *sv_new(t_symbol *s, long argc, t_atom *argv);
void sv_free(t_sv *x);
t_string* sv_get_path_to_external(t_class* c, char* subpath);
void sv_assist(t_sv *x, void *b, long m, long a, char *s);
void sv_dsp64(t_sv *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void sv_perform64(t_sv *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);

// message handlers
void svmax_load(t_sv *x, t_symbol *s);
void svmax_play(t_sv *x);
void svmax_stop(t_sv *x);
void svmax_volume(t_sv *x, long vol);


// global class pointer variable
static t_class *sv_class = NULL;


//***********************************************************************************************

void ext_main(void *r)
{
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.

	t_class *c = class_new("sunvox~", (method)sv_new, (method)dsp_free, (long)sizeof(t_sv), 0L, A_GIMME, 0);

	class_addmethod(c, (method)svmax_load,   "load",     A_SYM,   0);
	class_addmethod(c, (method)svmax_play,   "play",              0);
	class_addmethod(c, (method)svmax_stop,   "stop",              0);
	class_addmethod(c, (method)svmax_volume, "volume",   A_LONG,  0);
	class_addmethod(c, (method)sv_dsp64,    "dsp64",    A_CANT,  0);
	class_addmethod(c, (method)sv_assist,   "assist",   A_CANT,  0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	sv_class = c;
}

t_string* sv_get_path_to_external(t_class* c, char* subpath)
{
    char external_path[MAX_PATH_CHARS];
    char external_name[MAX_PATH_CHARS];
    short path_id = class_getpath(c);
    t_string* result;

#ifdef __APPLE__
    const char* ext_filename = "%s.mxo";
#else
    const char* ext_filename = "%s.mxe64";
#endif
    snprintf_zero(external_name, MAX_FILENAME_CHARS, ext_filename,
                  c->c_sym->s_name);
    path_toabsolutesystempath(path_id, external_name, external_path);
    result = string_new(external_path);
    if (subpath != NULL) {
        string_append(result, subpath);
    }
    return result;
}


void *sv_new(t_symbol *s, long argc, t_atom *argv)
{
	t_sv *x = (t_sv *)object_alloc(sv_class);

	if (x) {
		dsp_setup((t_pxobject *)x, N_IN_CHANNELS);	// MSP inlets: stereo input
		for (int i = 0; i < N_OUT_CHANNELS; i++) {
			outlet_new(x, "signal"); 	// signal outlets for stereo output
		}
		x->is_initialized = 0;
        x->in_sv_buffer = NULL;
        x->out_sv_buffer = NULL;
#if defined(__APPLE__)
        x->resources_dir = string_getptr(
            sv_get_path_to_external(sv_class, "/Contents/Resources"));
#else
        x->resources_dir = NULL;
#endif
	}
	return (x);
}


void sv_free(t_sv *x)
{
    delete[] x->in_sv_buffer;
    delete[] x->out_sv_buffer;
    if (x->is_initialized) {
        sv_close_slot(0);
        sv_deinit();
    }
    dsp_free((t_pxobject *)x);
}




void sv_assist(t_sv *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "I am inlet %ld", a);
	}
	else {	// outlet
		sprintf(s, "I am outlet %ld", a);
	}
}


void svmax_load(t_sv *x, t_symbol *s)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized, turn on audio first");
        return;
    }

    char path[MAX_PATH_CHARS];
    const char *filename = s->s_name;

    // If filename doesn't contain a path separator, look in resources dir
    if (strchr(filename, '/') == NULL && x->resources_dir != NULL) {
        snprintf_zero(path, MAX_PATH_CHARS, "%s/%s", x->resources_dir, filename);
    } else {
        snprintf_zero(path, MAX_PATH_CHARS, "%s", filename);
    }

    post("sunvox~: loading %s", path);

    sv_lock_slot(0);
    int res = sv_load(0, path);
    sv_unlock_slot(0);

    if (res == 0) {
        post("sunvox~: loaded '%s'", sv_get_song_name(0));
    } else {
        error("sunvox~: load error %d for %s", res, path);
    }
}


void svmax_play(t_sv *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized, turn on audio first");
        return;
    }
    sv_play_from_beginning(0);
}


void svmax_stop(t_sv *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_stop(0);
}


void svmax_volume(t_sv *x, long vol)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized, turn on audio first");
        return;
    }
    // Clamp to valid range 0-256
    if (vol < 0) vol = 0;
    if (vol > 256) vol = 256;
    sv_volume(0, (int)vol);
}


void sv_dsp64(t_sv *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // post("sample rate: %f", samplerate);
    // post("maxvectorsize: %d", maxvectorsize);

	if (x->is_initialized) {
		post("calling sv_deinit()");
        sv_close_slot( 0 );
        sv_deinit();
    }

    delete[] x->in_sv_buffer;
    delete[] x->out_sv_buffer;

    x->in_sv_buffer = new float[maxvectorsize * N_IN_CHANNELS];
    x->out_sv_buffer = new float[maxvectorsize * N_OUT_CHANNELS];

    memset(x->in_sv_buffer, 0.f, sizeof(float) * maxvectorsize * N_IN_CHANNELS);
    memset(x->out_sv_buffer, 0.f, sizeof(float) * maxvectorsize * N_OUT_CHANNELS);

    int ver = sv_init( 0, samplerate, N_OUT_CHANNELS, SV_INIT_FLAG_USER_AUDIO_CALLBACK
                                                    | SV_INIT_FLAG_AUDIO_FLOAT32
                                                    | SV_INIT_FLAG_ONE_THREAD);
    if( ver >= 0 )
    {
    	x->is_initialized = 1;
        sv_open_slot( 0 );
        /*
        SunVox is initialized.
        Slot 0 is open and ready for use.
        Then you can load and play some files in this slot.
        */
        post("sv_init successuflly called");
    } else {
    	error("sunvox init failed!");
    }
    object_method(dsp64, gensym("dsp_add64"), x, sv_perform64, 0, NULL);
}


void sv_perform64(t_sv *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts,
                           long sampleframes, long flags, void *userparam)
{
    float * in_ptr = x->in_sv_buffer;
    float * out_ptr = x->out_sv_buffer;
    int n = sampleframes;

    // Interleave input: Max provides separate channel buffers, SunVox expects interleaved
    // Buffer format: L0, R0, L1, R1, L2, R2, ...
    for (int i = 0; i < n; i++) {
        for (int chan = 0; chan < N_IN_CHANNELS; chan++) {
            if (chan < numins) {
                *(in_ptr++) = (float)ins[chan][i];
            } else {
                *(in_ptr++) = 0.0f;
            }
        }
    }

    sv_audio_callback2(x->out_sv_buffer, n, LATENCY, sv_get_ticks(), FLOAT32_TYPE, N_IN_CHANNELS, x->in_sv_buffer);

    // De-interleave output: SunVox provides interleaved, Max expects separate channel buffers
    for (int i = 0; i < n; i++) {
        for (int chan = 0; chan < N_OUT_CHANNELS; chan++) {
            if (chan < numouts) {
                outs[chan][i] = (double)(*out_ptr++);
            } else {
                out_ptr++;
            }
        }
    }
}



