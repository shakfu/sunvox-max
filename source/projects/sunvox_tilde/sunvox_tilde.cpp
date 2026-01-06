/**
	@file
	svm_tilde~: an external for the sunvox sound library
    by: Shakeeb Alireza
*/

#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects

#include <dlfcn.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SUNVOX_STATIC_LIB
#include <sunvox.h>

#define N_IN_CHANNELS 2
#define N_OUT_CHANNELS 2
#define FLOAT32_TYPE 1
#define LATENCY 0

// struct to represent the object's state
typedef struct _svm {
	t_pxobject		ob;	            // the object itself (t_pxobject in MSP instead of t_object)
    int is_initialized;             // flag to indicate if sv_init has been successfully called
    int is_loaded;                  // flag to indicate if a song is loaded
    int is_playing;                 // flag to track play state (for pause/resume)
    const char* resources_dir;      // resource directory inside external bundle
    t_symbol* song_filename;        // song filename (e.g song.sunvox) as symbol
    char filepath[MAX_PATH_CHARS];  // song path (full path) 
    char filename[MAX_PATH_CHARS];  // song filename
    short path_id;                  // song path id
	float *in_svm_buffer;           // intermediate sunvox input buffer
    float *out_svm_buffer;          // intermediate sunvox output buffer
    int last_line;                  // last reported line (to detect song end)
} t_svm;


// method prototypes
void *svm_new(t_symbol *s, long argc, t_atom *argv);
void svm_free(t_svm *x);
t_string* svm_get_path_to_external(t_class* c, char* subpath);
bool svm_load_file(t_svm* x);
void svm_assist(t_svm *x, void *b, long m, long a, char *s);
void svm_dsp64(t_svm *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void svm_perform64(t_svm *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);

// message handlers
void svm_load(t_svm *x, t_symbol *s);
void svm_play(t_svm *x);
void svm_stop(t_svm *x);
void svm_volume(t_svm *x, long vol);
void svm_note(t_svm *x, t_symbol *s, long argc, t_atom *argv);


// global class pointer variable
static t_class *svm_class = NULL;


//***********************************************************************************************

void ext_main(void *r)
{
	t_class *c = class_new("sunvox~", (method)svm_new, (method)dsp_free, (long)sizeof(t_svm), 0L, A_GIMME, 0);

	class_addmethod(c, (method)svm_load,   "load",     A_SYM,   0);
	class_addmethod(c, (method)svm_play,   "play",              0);
	class_addmethod(c, (method)svm_stop,   "stop",              0);
	class_addmethod(c, (method)svm_volume, "volume",   A_LONG,  0);
    class_addmethod(c, (method)svm_note,   "note",     A_GIMME, 0);
	class_addmethod(c, (method)svm_dsp64,  "dsp64",    A_CANT,  0);
	class_addmethod(c, (method)svm_assist, "assist",   A_CANT,  0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	svm_class = c;
}

t_string* svm_get_path_to_external(t_class* c, char* subpath)
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


void *svm_new(t_symbol *s, long argc, t_atom *argv)
{
	t_svm *x = (t_svm *)object_alloc(svm_class);

	if (x) {
		dsp_setup((t_pxobject *)x, N_IN_CHANNELS);	// MSP inlets: stereo input
		for (int i = 0; i < N_OUT_CHANNELS; i++) {
			outlet_new(x, "signal"); 	// signal outlets for stereo output
		}
        x->is_initialized = 0;
        x->is_loaded = 0;
        x->is_playing = 0;
        x->last_line = -1;
        x->in_svm_buffer = NULL;
        x->out_svm_buffer = NULL;
#if defined(__APPLE__)
        x->resources_dir = string_getptr(
            svm_get_path_to_external(svm_class, "/Contents/Resources"));
#else
        x->resources_dir = NULL;
#endif
        x->path_id = 0;
        // Get song filename (first argument)
        if (argc > 0 && atom_gettype(argv) == A_SYM) {
            x->song_filename = atom_getsym(argv);
        } else {
            x->song_filename = gensym("");
        }
	}
	return (x);
}


void svm_free(t_svm *x)
{
    delete[] x->in_svm_buffer;
    delete[] x->out_svm_buffer;
    if (x->is_initialized) {
        sv_close_slot(0);
        sv_deinit();
    }
    dsp_free((t_pxobject *)x);
}




void svm_assist(t_svm *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "I am inlet %ld", a);
	}
	else {	// outlet
		sprintf(s, "I am outlet %ld", a);
	}
}

bool svm_load_file(t_svm* x)
{
    char filename[MAX_PATH_CHARS];
    char filepath[MAX_PATH_CHARS];
    t_fourcc outtype = 0;
    short path_id;
    t_max_err err;

    // post("song_filename: %s", x->song_filename->s_name);
    strncpy_zero(filename, x->song_filename->s_name, MAX_PATH_CHARS);

    // Locate file in Max search path: (NULL,0 mean any type)
    if (locatefile_extended(filename, &path_id, &outtype, NULL, 0)) {
        error("svm_load_file: cannot find file %s", x->song_filename->s_name);
        return false;
    }

    // post("outtype: %d", outtype);

    // Get absolute path
    filepath[0] = '\0';
    err = path_toabsolutesystempath(path_id, filename, filepath);
    if (err != MAX_ERR_NONE) {
        error("svm_load_file: cannot convert %s to absolute path", x->song_filename->s_name);
        return false;
    }

    // Store paths
    x->path_id = path_id;
    strncpy(x->filepath, filepath, MAX_PATH_CHARS - 1);
    x->filepath[MAX_PATH_CHARS - 1] = '\0';
    strncpy(x->filename, filename, MAX_PATH_CHARS - 1);
    x->filename[MAX_PATH_CHARS - 1] = '\0';

    // find
    post("file: found %s", filepath);
    return true;
}



void svm_load(t_svm *x, t_symbol *s)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized, turn on audio first");
        return;
    }

    x->song_filename = s;

    if(svm_load_file(x)) {
        post("sunvox~: loading %s", x->filepath);
    } else {
        error("sunvox~: could not load");
        return;
    }

    sv_lock_slot(0);
    int res = sv_load(0, x->filepath);
    sv_unlock_slot(0);

    if (res == 0) {
        x->is_loaded = 1;
        x->is_playing = 0;
        x->last_line = -1;
        post("sunvox~: loaded '%s'", sv_get_song_name(0));
    } else {
        x->is_loaded = 0;
        error("sunvox~: load error %d for %s", res, x->filepath);
    }
}


// void svm_load(t_svm *x, t_symbol *s)
// {
//     if (!x->is_initialized) {
//         error("sunvox~: not initialized, turn on audio first");
//         return;
//     }

//     char path[MAX_PATH_CHARS];
//     const char *filename = s->s_name;

//     // If filename doesn't contain a path separator, look in resources dir
//     if (strchr(filename, '/') == NULL && x->resources_dir != NULL) {
//         snprintf_zero(path, MAX_PATH_CHARS, "%s/%s", x->resources_dir, filename);
//     } else {
//         snprintf_zero(path, MAX_PATH_CHARS, "%s", filename);
//     }

//     post("sunvox~: loading %s", path);

//     sv_lock_slot(0);
//     int res = sv_load(0, path);
//     sv_unlock_slot(0);

//     if (res == 0) {
//         post("sunvox~: loaded '%s'", sv_get_song_name(0));
//     } else {
//         error("sunvox~: load error %d for %s", res, path);
//     }
// }

void svm_play(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized, turn on audio first");
        return;
    }
    if (!x->is_loaded) {
        error("sunvox~: no song loaded");
        return;
    }
    post("sunvox~: starting playback...");
    sv_play_from_beginning(0);
    x->is_playing = 1;
    x->last_line = -1;
    // Check if playback started
    post("sunvox~: playing, current line = %d", sv_get_current_line(0));
}


void svm_stop(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_stop(0);
}


void svm_volume(t_svm *x, long vol)
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


void svm_note(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: note <module> <note> [velocity] [track]
    // module: module number or name
    // note: MIDI note number (0-127) or -1 for note off
    // velocity: 1-129 (default 129)
    // track: track number (default 0)

    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 2) {
        error("sunvox~: note requires at least module and note arguments");
        return;
    }

    int module = 0;
    int note = 0;
    int velocity = 129;  // Max velocity
    int track = 0;

    // Get module (can be number or name)
    if (atom_gettype(argv) == A_LONG) {
        module = (int)atom_getlong(argv) + 1;  // SunVox modules are 1-indexed in events
    } else if (atom_gettype(argv) == A_SYM) {
        module = sv_find_module(0, atom_getsym(argv)->s_name) + 1;
        if (module <= 0) {
            error("sunvox~: module '%s' not found", atom_getsym(argv)->s_name);
            return;
        }
    }

    // Get note
    note = (int)atom_getlong(argv + 1);
    if (note < 0) {
        note = NOTECMD_NOTE_OFF;
    }

    // Optional velocity
    if (argc > 2) {
        velocity = (int)atom_getlong(argv + 2);
        if (velocity < 1) velocity = 1;
        if (velocity > 129) velocity = 129;
    }

    // Optional track
    if (argc > 3) {
        track = (int)atom_getlong(argv + 3);
    }

    sv_send_event(0, track, note, velocity, module, 0, 0);
}


void svm_dsp64(t_svm *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // post("sample rate: %f", samplerate);
    // post("maxvectorsize: %d", maxvectorsize);

	if (x->is_initialized) {
		post("calling sv_deinit()");
        sv_close_slot( 0 );
        sv_deinit();
    }

    delete[] x->in_svm_buffer;
    delete[] x->out_svm_buffer;

    x->in_svm_buffer = new float[maxvectorsize * N_IN_CHANNELS];
    x->out_svm_buffer = new float[maxvectorsize * N_OUT_CHANNELS];

    memset(x->in_svm_buffer, 0.f, sizeof(float) * maxvectorsize * N_IN_CHANNELS);
    memset(x->out_svm_buffer, 0.f, sizeof(float) * maxvectorsize * N_OUT_CHANNELS);

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
    object_method(dsp64, gensym("dsp_add64"), x, svm_perform64, 0, NULL);
}


void svm_perform64(t_svm *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts,
                           long sampleframes, long flags, void *userparam)
{
    float * in_ptr = x->in_svm_buffer;
    float * out_ptr = x->out_svm_buffer;
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

    sv_audio_callback2(x->out_svm_buffer, n, LATENCY, sv_get_ticks(), FLOAT32_TYPE, N_IN_CHANNELS, x->in_svm_buffer);

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



