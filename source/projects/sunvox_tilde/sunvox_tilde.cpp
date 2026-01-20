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
    t_atom_long volume;             // volume attribute (0-256)
    t_atom_long autostop;           // autostop attribute (0/1)
    const char* resources_dir;      // resource directory inside external bundle
    t_symbol* song_filename;        // song filename (e.g song.sunvox) as symbol
    char filepath[MAX_PATH_CHARS];  // song path (full path)
    char filename[MAX_PATH_CHARS];  // song filename
    short path_id;                  // song path id
	float *in_svm_buffer;           // intermediate sunvox input buffer
    float *out_svm_buffer;          // intermediate sunvox output buffer
    long buffer_size;               // current buffer size
    int last_line;                  // last reported line (to detect song end)
    void* info_outlet;              // info outlet for query responses
} t_svm;


// method prototypes
void *svm_new(t_symbol *s, long argc, t_atom *argv);
void svm_free(t_svm *x);
t_string* svm_get_path_to_external(t_class* c, char* subpath);
bool svm_load_file(t_svm* x);
void svm_assist(t_svm *x, void *b, long m, long a, char *s);
void svm_dsp64(t_svm *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void svm_perform64(t_svm *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);

// File I/O
void svm_load(t_svm *x, t_symbol *s);
void svm_read(t_svm *x, t_symbol *s);
void svm_save(t_svm *x, t_symbol *s);
void svm_write(t_svm *x, t_symbol *s);

// Playback control
void svm_play(t_svm *x);
void svm_playfrom(t_svm *x);
void svm_stop(t_svm *x);
void svm_pause(t_svm *x);
void svm_resume(t_svm *x);
void svm_rewind(t_svm *x, t_atom_long line);
void svm_autostop_msg(t_svm *x, t_atom_long val);
void svm_volume(t_svm *x, long vol);

// Song info queries
void svm_getinfo(t_svm *x);
void svm_getname(t_svm *x);
void svm_setname(t_svm *x, t_symbol *s);
void svm_getbpm(t_svm *x);
void svm_gettpl(t_svm *x);
void svm_getlength(t_svm *x);
void svm_getline(t_svm *x);
void svm_getlevel(t_svm *x, t_atom_long chan);
void svm_endofsong(t_svm *x);

// Events
void svm_note(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_noteoff(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_allnotesoff(t_svm *x);
void svm_cleansynths(t_svm *x);
void svm_event(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_ctl(t_svm *x, t_symbol *s, long argc, t_atom *argv);

// Module management
void svm_newmodule(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_removemodule(t_svm *x, t_atom_long mod);
void svm_connect(t_svm *x, t_atom_long src, t_atom_long dst);
void svm_disconnect(t_svm *x, t_atom_long src, t_atom_long dst);
void svm_loadmodule(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_findmodule(t_svm *x, t_symbol *s);
void svm_getmodule(t_svm *x, t_atom_long mod);
void svm_modules(t_svm *x);
void svm_setmodulename(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_setmodulexy(t_svm *x, t_atom_long mod, t_atom_long xpos, t_atom_long ypos);
void svm_setmodulecolor(t_svm *x, t_atom_long mod, t_atom_long color);
void svm_setfinetune(t_svm *x, t_atom_long mod, t_atom_long val);
void svm_setrelnote(t_svm *x, t_atom_long mod, t_atom_long val);

// Controller queries
void svm_getctl(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_getctls(t_svm *x, t_atom_long mod);

// Sampler/MetaModule
void svm_samplerload(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_metamoduleload(t_svm *x, t_atom_long mod, t_symbol *s);
void svm_vplayerload(t_svm *x, t_atom_long mod, t_symbol *s);

// Pattern management
void svm_newpattern(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_removepattern(t_svm *x, t_atom_long pat);
void svm_findpattern(t_svm *x, t_symbol *s);
void svm_getpattern(t_svm *x, t_atom_long pat);
void svm_patterns(t_svm *x);
void svm_setpatternxy(t_svm *x, t_atom_long pat, t_atom_long xpos, t_atom_long ypos);
void svm_setpatternsize(t_svm *x, t_atom_long pat, t_atom_long tracks, t_atom_long lines);
void svm_setpatternname(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_patternmute(t_svm *x, t_atom_long pat, t_atom_long mute);
void svm_setpatternevent(t_svm *x, t_symbol *s, long argc, t_atom *argv);
void svm_getpatternevent(t_svm *x, t_symbol *s, long argc, t_atom *argv);

// Utility
void svm_log(t_svm *x, t_atom_long size);
void svm_samplerate(t_svm *x);
void svm_version(t_svm *x);

// Helper for resolving module number from int or name
int svm_resolve_module(t_svm *x, t_atom *a);
bool svm_locate_file(t_svm *x, const char *filename, char *outpath);


// global class pointer variable
static t_class *svm_class = NULL;


//***********************************************************************************************

void ext_main(void *r)
{
	t_class *c = class_new("sunvox~", (method)svm_new, (method)svm_free, (long)sizeof(t_svm), 0L, A_GIMME, 0);

    // File I/O
	class_addmethod(c, (method)svm_load,   "load",     A_SYM,   0);
    class_addmethod(c, (method)svm_read,   "read",     A_SYM,   0);
    class_addmethod(c, (method)svm_save,   "save",     A_SYM,   0);
    class_addmethod(c, (method)svm_write,  "write",    A_SYM,   0);

    // Playback control
	class_addmethod(c, (method)svm_play,   "play",              0);
    class_addmethod(c, (method)svm_playfrom, "playfrom",        0);
	class_addmethod(c, (method)svm_stop,   "stop",              0);
    class_addmethod(c, (method)svm_pause,  "pause",             0);
    class_addmethod(c, (method)svm_resume, "resume",            0);
    class_addmethod(c, (method)svm_rewind, "rewind",   A_LONG,  0);
    class_addmethod(c, (method)svm_autostop_msg, "autostop", A_LONG, 0);
	class_addmethod(c, (method)svm_volume, "volume",   A_LONG,  0);

    // Song info queries
    class_addmethod(c, (method)svm_getinfo,   "getinfo",         0);
    class_addmethod(c, (method)svm_getname,   "getname",         0);
    class_addmethod(c, (method)svm_setname,   "setname",  A_SYM, 0);
    class_addmethod(c, (method)svm_getbpm,    "getbpm",          0);
    class_addmethod(c, (method)svm_gettpl,    "gettpl",          0);
    class_addmethod(c, (method)svm_getlength, "getlength",       0);
    class_addmethod(c, (method)svm_getline,   "getline",         0);
    class_addmethod(c, (method)svm_getlevel,  "getlevel", A_LONG, 0);
    class_addmethod(c, (method)svm_endofsong, "endofsong",       0);

    // Events
    class_addmethod(c, (method)svm_note,        "note",        A_GIMME, 0);
    class_addmethod(c, (method)svm_noteoff,     "noteoff",     A_GIMME, 0);
    class_addmethod(c, (method)svm_allnotesoff, "allnotesoff",          0);
    class_addmethod(c, (method)svm_cleansynths, "cleansynths",          0);
    class_addmethod(c, (method)svm_event,       "event",       A_GIMME, 0);
    class_addmethod(c, (method)svm_ctl,         "ctl",         A_GIMME, 0);

    // Module management
    class_addmethod(c, (method)svm_newmodule,      "newmodule",      A_GIMME, 0);
    class_addmethod(c, (method)svm_removemodule,   "removemodule",   A_LONG,  0);
    class_addmethod(c, (method)svm_connect,        "connect",        A_LONG, A_LONG, 0);
    class_addmethod(c, (method)svm_disconnect,     "disconnect",     A_LONG, A_LONG, 0);
    class_addmethod(c, (method)svm_loadmodule,     "loadmodule",     A_GIMME, 0);
    class_addmethod(c, (method)svm_findmodule,     "findmodule",     A_SYM,   0);
    class_addmethod(c, (method)svm_getmodule,      "getmodule",      A_LONG,  0);
    class_addmethod(c, (method)svm_modules,        "modules",                 0);
    class_addmethod(c, (method)svm_setmodulename,  "setmodulename",  A_GIMME, 0);
    class_addmethod(c, (method)svm_setmodulexy,    "setmodulexy",    A_LONG, A_LONG, A_LONG, 0);
    class_addmethod(c, (method)svm_setmodulecolor, "setmodulecolor", A_LONG, A_LONG, 0);
    class_addmethod(c, (method)svm_setfinetune,    "setfinetune",    A_LONG, A_LONG, 0);
    class_addmethod(c, (method)svm_setrelnote,     "setrelnote",     A_LONG, A_LONG, 0);

    // Controller queries
    class_addmethod(c, (method)svm_getctl,  "getctl",  A_GIMME, 0);
    class_addmethod(c, (method)svm_getctls, "getctls", A_LONG,  0);

    // Sampler/MetaModule
    class_addmethod(c, (method)svm_samplerload,    "samplerload",    A_GIMME, 0);
    class_addmethod(c, (method)svm_metamoduleload, "metamoduleload", A_LONG, A_SYM, 0);
    class_addmethod(c, (method)svm_vplayerload,    "vplayerload",    A_LONG, A_SYM, 0);

    // Pattern management
    class_addmethod(c, (method)svm_newpattern,      "newpattern",      A_GIMME, 0);
    class_addmethod(c, (method)svm_removepattern,   "removepattern",   A_LONG,  0);
    class_addmethod(c, (method)svm_findpattern,     "findpattern",     A_SYM,   0);
    class_addmethod(c, (method)svm_getpattern,      "getpattern",      A_LONG,  0);
    class_addmethod(c, (method)svm_patterns,        "patterns",                 0);
    class_addmethod(c, (method)svm_setpatternxy,    "setpatternxy",    A_LONG, A_LONG, A_LONG, 0);
    class_addmethod(c, (method)svm_setpatternsize,  "setpatternsize",  A_LONG, A_LONG, A_LONG, 0);
    class_addmethod(c, (method)svm_setpatternname,  "setpatternname",  A_GIMME, 0);
    class_addmethod(c, (method)svm_patternmute,     "patternmute",     A_LONG, A_LONG, 0);
    class_addmethod(c, (method)svm_setpatternevent, "setpatternevent", A_GIMME, 0);
    class_addmethod(c, (method)svm_getpatternevent, "getpatternevent", A_GIMME, 0);

    // Utility
    class_addmethod(c, (method)svm_log,        "log",        A_DEFLONG, 0);
    class_addmethod(c, (method)svm_samplerate, "samplerate",            0);
    class_addmethod(c, (method)svm_version,    "version",               0);

    // DSP
	class_addmethod(c, (method)svm_dsp64,  "dsp64",    A_CANT,  0);
	class_addmethod(c, (method)svm_assist, "assist",   A_CANT,  0);

    // Attributes
    CLASS_ATTR_LONG(c, "volume", 0, t_svm, volume);
    CLASS_ATTR_ACCESSORS(c, "volume", NULL, NULL);
    CLASS_ATTR_LABEL(c, "volume", 0, "Volume (0-256)");
    CLASS_ATTR_FILTER_CLIP(c, "volume", 0, 256);
    CLASS_ATTR_DEFAULT(c, "volume", 0, "256");
    CLASS_ATTR_SAVE(c, "volume", 0);

    CLASS_ATTR_LONG(c, "autostop", 0, t_svm, autostop);
    CLASS_ATTR_ACCESSORS(c, "autostop", NULL, NULL);
    CLASS_ATTR_LABEL(c, "autostop", 0, "Auto-stop at end of song");
    CLASS_ATTR_STYLE(c, "autostop", 0, "onoff");
    CLASS_ATTR_DEFAULT(c, "autostop", 0, "0");
    CLASS_ATTR_SAVE(c, "autostop", 0);

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

        // Create info outlet first (rightmost, outlet 2)
        x->info_outlet = outlet_new((t_object *)x, NULL);

        // Create signal outlets after (outlets 1, 0 from right to left)
		for (int i = 0; i < N_OUT_CHANNELS; i++) {
			outlet_new(x, "signal");
		}

        x->is_initialized = 0;
        x->is_loaded = 0;
        x->is_playing = 0;
        x->volume = 256;
        x->autostop = 0;
        x->last_line = -1;
        x->in_svm_buffer = NULL;
        x->out_svm_buffer = NULL;
        x->buffer_size = 0;
#if defined(__APPLE__)
        x->resources_dir = string_getptr(
            svm_get_path_to_external(svm_class, "/Contents/Resources"));
#else
        x->resources_dir = NULL;
#endif
        x->path_id = 0;

        // Process attributes first
        attr_args_process(x, argc, argv);

        // Get song filename (first non-attribute argument)
        long attrstart = attr_args_offset(argc, argv);
        if (attrstart > 0 && atom_gettype(argv) == A_SYM) {
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
	if (m == ASSIST_INLET) {
        switch (a) {
            case 0: sprintf(s, "(signal) Left audio input"); break;
            case 1: sprintf(s, "(signal) Right audio input"); break;
            default: sprintf(s, "Inlet %ld", a); break;
        }
	}
	else {
        switch (a) {
            case 0: sprintf(s, "(signal) Left audio output"); break;
            case 1: sprintf(s, "(signal) Right audio output"); break;
            case 2: sprintf(s, "Info outlet (query responses)"); break;
            default: sprintf(s, "Outlet %ld", a); break;
        }
	}
}


// Helper to locate a file in Max search path
bool svm_locate_file(t_svm *x, const char *filename, char *outpath)
{
    char fname[MAX_PATH_CHARS];
    t_fourcc outtype = 0;
    short path_id;
    t_max_err err;

    strncpy_zero(fname, filename, MAX_PATH_CHARS);

    // Locate file in Max search path
    if (locatefile_extended(fname, &path_id, &outtype, NULL, 0)) {
        return false;
    }

    // Get absolute path
    outpath[0] = '\0';
    err = path_toabsolutesystempath(path_id, fname, outpath);
    if (err != MAX_ERR_NONE) {
        return false;
    }

    return true;
}


bool svm_load_file(t_svm* x)
{
    char filename[MAX_PATH_CHARS];
    char filepath[MAX_PATH_CHARS];
    t_fourcc outtype = 0;
    short path_id;
    t_max_err err;

    strncpy_zero(filename, x->song_filename->s_name, MAX_PATH_CHARS);

    // Locate file in Max search path
    if (locatefile_extended(filename, &path_id, &outtype, NULL, 0)) {
        error("sunvox~: cannot find file %s", x->song_filename->s_name);
        return false;
    }

    // Get absolute path
    filepath[0] = '\0';
    err = path_toabsolutesystempath(path_id, filename, filepath);
    if (err != MAX_ERR_NONE) {
        error("sunvox~: cannot convert %s to absolute path", x->song_filename->s_name);
        return false;
    }

    // Store paths
    x->path_id = path_id;
    strncpy(x->filepath, filepath, MAX_PATH_CHARS - 1);
    x->filepath[MAX_PATH_CHARS - 1] = '\0';
    strncpy(x->filename, filename, MAX_PATH_CHARS - 1);
    x->filename[MAX_PATH_CHARS - 1] = '\0';

    return true;
}


// Helper to resolve module number from atom (int or symbol name)
int svm_resolve_module(t_svm *x, t_atom *a)
{
    if (atom_gettype(a) == A_LONG) {
        return (int)atom_getlong(a);
    } else if (atom_gettype(a) == A_SYM) {
        int mod = sv_find_module(0, atom_getsym(a)->s_name);
        if (mod < 0) {
            error("sunvox~: module '%s' not found", atom_getsym(a)->s_name);
        }
        return mod;
    }
    return -1;
}


//==============================================================================
// File I/O
//==============================================================================

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
        // Apply current volume and autostop settings
        sv_volume(0, (int)x->volume);
        sv_set_autostop(0, (int)x->autostop);
        post("sunvox~: loaded '%s'", sv_get_song_name(0));

        // Output loaded message to info outlet
        t_atom av[1];
        atom_setsym(av, gensym(sv_get_song_name(0)));
        outlet_anything(x->info_outlet, gensym("loaded"), 1, av);
    } else {
        x->is_loaded = 0;
        error("sunvox~: load error %d for %s", res, x->filepath);
    }
}


void svm_read(t_svm *x, t_symbol *s)
{
    svm_load(x, s);
}


void svm_save(t_svm *x, t_symbol *s)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (!x->is_loaded) {
        error("sunvox~: no song loaded");
        return;
    }

    char filepath[MAX_PATH_CHARS];
    const char *filename = s->s_name;

    // If it's just a filename, save to same directory as loaded file
    if (strchr(filename, '/') == NULL && x->path_id != 0) {
        path_toabsolutesystempath(x->path_id, (char*)filename, filepath);
    } else {
        strncpy_zero(filepath, filename, MAX_PATH_CHARS);
    }

    int res = sv_save(0, filepath);
    if (res == 0) {
        post("sunvox~: saved to %s", filepath);
        t_atom av[1];
        atom_setsym(av, s);
        outlet_anything(x->info_outlet, gensym("saved"), 1, av);
    } else {
        error("sunvox~: save error %d for %s", res, filepath);
    }
}


void svm_write(t_svm *x, t_symbol *s)
{
    svm_save(x, s);
}


//==============================================================================
// Playback Control
//==============================================================================

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
    sv_play(0);
    x->is_playing = 1;
    outlet_anything(x->info_outlet, gensym("playing"), 0, NULL);
}


void svm_playfrom(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized, turn on audio first");
        return;
    }
    if (!x->is_loaded) {
        error("sunvox~: no song loaded");
        return;
    }
    sv_play_from_beginning(0);
    x->is_playing = 1;
    x->last_line = -1;
    outlet_anything(x->info_outlet, gensym("playing"), 0, NULL);
}


void svm_stop(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_stop(0);
    x->is_playing = 0;
    outlet_anything(x->info_outlet, gensym("stopped"), 0, NULL);
}


void svm_pause(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_pause(0);
    x->is_playing = 0;
    outlet_anything(x->info_outlet, gensym("paused"), 0, NULL);
}


void svm_resume(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_resume(0);
    x->is_playing = 1;
    outlet_anything(x->info_outlet, gensym("playing"), 0, NULL);
}


void svm_rewind(t_svm *x, t_atom_long line)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_rewind(0, (int)line);
    x->last_line = (int)line;
}


void svm_autostop_msg(t_svm *x, t_atom_long val)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    x->autostop = val ? 1 : 0;
    sv_set_autostop(0, (int)x->autostop);
}


void svm_volume(t_svm *x, long vol)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized, turn on audio first");
        return;
    }
    if (vol < 0) vol = 0;
    if (vol > 256) vol = 256;
    x->volume = vol;
    sv_volume(0, (int)vol);
}


//==============================================================================
// Song Info Queries
//==============================================================================

void svm_getinfo(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    t_dictionary *d = dictionary_new();
    dictionary_appendstring(d, gensym("name"), sv_get_song_name(0));
    dictionary_appendlong(d, gensym("bpm"), sv_get_song_bpm(0));
    dictionary_appendlong(d, gensym("tpl"), sv_get_song_tpl(0));
    dictionary_appendlong(d, gensym("length_frames"), sv_get_song_length_frames(0));
    dictionary_appendlong(d, gensym("length_lines"), sv_get_song_length_lines(0));
    dictionary_appendlong(d, gensym("current_line"), sv_get_current_line(0));
    dictionary_appendlong(d, gensym("modules"), sv_get_number_of_modules(0));
    dictionary_appendlong(d, gensym("patterns"), sv_get_number_of_patterns(0));
    dictionary_appendlong(d, gensym("playing"), !sv_end_of_song(0));
    dictionary_appendlong(d, gensym("autostop"), sv_get_autostop(0));

    t_atom av[1];
    atom_setobj(av, d);
    outlet_anything(x->info_outlet, gensym("info"), 1, av);
}


void svm_getname(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    t_atom av[1];
    atom_setsym(av, gensym(sv_get_song_name(0)));
    outlet_anything(x->info_outlet, gensym("name"), 1, av);
}


void svm_setname(t_svm *x, t_symbol *s)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_set_song_name(0, s->s_name);
}


void svm_getbpm(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    t_atom av[1];
    atom_setlong(av, sv_get_song_bpm(0));
    outlet_anything(x->info_outlet, gensym("bpm"), 1, av);
}


void svm_gettpl(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    t_atom av[1];
    atom_setlong(av, sv_get_song_tpl(0));
    outlet_anything(x->info_outlet, gensym("tpl"), 1, av);
}


void svm_getlength(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    t_atom av[2];
    atom_setlong(av, sv_get_song_length_frames(0));
    atom_setlong(av + 1, sv_get_song_length_lines(0));
    outlet_anything(x->info_outlet, gensym("length"), 2, av);
}


void svm_getline(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    t_atom av[1];
    atom_setlong(av, sv_get_current_line(0));
    outlet_anything(x->info_outlet, gensym("line"), 1, av);
}


void svm_getlevel(t_svm *x, t_atom_long chan)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    t_atom av[2];
    atom_setlong(av, chan);
    atom_setlong(av + 1, sv_get_current_signal_level(0, (int)chan));
    outlet_anything(x->info_outlet, gensym("level"), 2, av);
}


void svm_endofsong(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    t_atom av[1];
    atom_setlong(av, sv_end_of_song(0));
    outlet_anything(x->info_outlet, gensym("endofsong"), 1, av);
}


//==============================================================================
// Events
//==============================================================================

void svm_note(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: note <module> <note> [velocity] [track]
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 2) {
        error("sunvox~: note requires at least module and note arguments");
        return;
    }

    int module = svm_resolve_module(x, argv);
    if (module < 0) return;
    module += 1;  // SunVox modules are 1-indexed in events

    int note = (int)atom_getlong(argv + 1);
    if (note < 0) {
        note = NOTECMD_NOTE_OFF;
    }

    int velocity = 129;  // Max velocity
    if (argc > 2) {
        velocity = (int)atom_getlong(argv + 2);
        if (velocity < 1) velocity = 1;
        if (velocity > 129) velocity = 129;
    }

    int track = 0;
    if (argc > 3) {
        track = (int)atom_getlong(argv + 3);
    }

    sv_send_event(0, track, note, velocity, module, 0, 0);
}


void svm_noteoff(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: noteoff <module> [track]
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 1) {
        error("sunvox~: noteoff requires module argument");
        return;
    }

    int module = svm_resolve_module(x, argv);
    if (module < 0) return;
    module += 1;

    int track = 0;
    if (argc > 1) {
        track = (int)atom_getlong(argv + 1);
    }

    sv_send_event(0, track, NOTECMD_NOTE_OFF, 0, module, 0, 0);
}


void svm_allnotesoff(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_send_event(0, 0, NOTECMD_ALL_NOTES_OFF, 0, 0, 0, 0);
}


void svm_cleansynths(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_send_event(0, 0, NOTECMD_CLEAN_SYNTHS, 0, 0, 0, 0);
}


void svm_event(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: event <track> <note> <vel> <mod> <ctl> <ctl_val>
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 6) {
        error("sunvox~: event requires 6 arguments: track note vel mod ctl ctl_val");
        return;
    }

    int track = (int)atom_getlong(argv);
    int note = (int)atom_getlong(argv + 1);
    int vel = (int)atom_getlong(argv + 2);
    int mod = (int)atom_getlong(argv + 3) + 1;  // 1-indexed
    int ctl = (int)atom_getlong(argv + 4);
    int ctl_val = (int)atom_getlong(argv + 5);

    sv_send_event(0, track, note, vel, mod, ctl, ctl_val);
}


void svm_ctl(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: ctl <mod> <ctl> <val> [scaled]
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 3) {
        error("sunvox~: ctl requires module, controller, and value arguments");
        return;
    }

    int module = svm_resolve_module(x, argv);
    if (module < 0) return;

    int ctl = (int)atom_getlong(argv + 1);
    int val = (int)atom_getlong(argv + 2);
    int scaled = 0;
    if (argc > 3) {
        scaled = (int)atom_getlong(argv + 3);
    }

    sv_set_module_ctl_value(0, module, ctl, val, scaled);
}


//==============================================================================
// Module Management
//==============================================================================

void svm_newmodule(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: newmodule <type> <name> [x y z]
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 2) {
        error("sunvox~: newmodule requires type and name arguments");
        return;
    }

    const char *type = atom_getsym(argv)->s_name;
    const char *name = atom_getsym(argv + 1)->s_name;
    int xpos = 512, ypos = 512, z = 0;

    if (argc > 2) xpos = (int)atom_getlong(argv + 2);
    if (argc > 3) ypos = (int)atom_getlong(argv + 3);
    if (argc > 4) z = (int)atom_getlong(argv + 4);

    sv_lock_slot(0);
    int mod = sv_new_module(0, type, name, xpos, ypos, z);
    sv_unlock_slot(0);

    if (mod >= 0) {
        t_atom av[1];
        atom_setlong(av, mod);
        outlet_anything(x->info_outlet, gensym("newmodule"), 1, av);
    } else {
        error("sunvox~: failed to create module of type '%s'", type);
    }
}


void svm_removemodule(t_svm *x, t_atom_long mod)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    sv_lock_slot(0);
    int res = sv_remove_module(0, (int)mod);
    sv_unlock_slot(0);

    if (res != 0) {
        error("sunvox~: failed to remove module %ld", mod);
    }
}


void svm_connect(t_svm *x, t_atom_long src, t_atom_long dst)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    sv_lock_slot(0);
    int res = sv_connect_module(0, (int)src, (int)dst);
    sv_unlock_slot(0);

    if (res != 0) {
        error("sunvox~: failed to connect module %ld to %ld", src, dst);
    }
}


void svm_disconnect(t_svm *x, t_atom_long src, t_atom_long dst)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    sv_lock_slot(0);
    int res = sv_disconnect_module(0, (int)src, (int)dst);
    sv_unlock_slot(0);

    if (res != 0) {
        error("sunvox~: failed to disconnect module %ld from %ld", src, dst);
    }
}


void svm_loadmodule(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: loadmodule <file> [x y z]
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 1) {
        error("sunvox~: loadmodule requires filename argument");
        return;
    }

    const char *filename = atom_getsym(argv)->s_name;
    char filepath[MAX_PATH_CHARS];

    if (!svm_locate_file(x, filename, filepath)) {
        error("sunvox~: cannot find file %s", filename);
        return;
    }

    int xpos = 512, ypos = 512, z = 0;
    if (argc > 1) xpos = (int)atom_getlong(argv + 1);
    if (argc > 2) ypos = (int)atom_getlong(argv + 2);
    if (argc > 3) z = (int)atom_getlong(argv + 3);

    int mod = sv_load_module(0, filepath, xpos, ypos, z);
    if (mod >= 0) {
        t_atom av[1];
        atom_setlong(av, mod);
        outlet_anything(x->info_outlet, gensym("loadmodule"), 1, av);
    } else {
        error("sunvox~: failed to load module from %s", filepath);
    }
}


void svm_findmodule(t_svm *x, t_symbol *s)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    int mod = sv_find_module(0, s->s_name);
    t_atom av[1];
    atom_setlong(av, mod);
    outlet_anything(x->info_outlet, gensym("findmodule"), 1, av);
}


void svm_getmodule(t_svm *x, t_atom_long mod)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    uint32_t flags = sv_get_module_flags(0, (int)mod);
    if (!(flags & SV_MODULE_FLAG_EXISTS)) {
        error("sunvox~: module %ld does not exist", mod);
        return;
    }

    const char *name = sv_get_module_name(0, (int)mod);
    const char *type = sv_get_module_type(0, (int)mod);
    uint32_t xy = sv_get_module_xy(0, (int)mod);
    int mx, my;
    SV_GET_MODULE_XY(xy, mx, my);
    int color = sv_get_module_color(0, (int)mod);
    int num_ctls = sv_get_number_of_module_ctls(0, (int)mod);
    int num_inputs = (flags & SV_MODULE_INPUTS_MASK) >> SV_MODULE_INPUTS_OFF;
    int num_outputs = (flags & SV_MODULE_OUTPUTS_MASK) >> SV_MODULE_OUTPUTS_OFF;

    t_dictionary *d = dictionary_new();
    dictionary_appendlong(d, gensym("id"), mod);
    dictionary_appendstring(d, gensym("name"), name ? name : "");
    dictionary_appendstring(d, gensym("type"), type ? type : "");
    dictionary_appendlong(d, gensym("x"), mx);
    dictionary_appendlong(d, gensym("y"), my);
    dictionary_appendlong(d, gensym("color"), color);
    dictionary_appendlong(d, gensym("controllers"), num_ctls);
    dictionary_appendlong(d, gensym("inputs"), num_inputs);
    dictionary_appendlong(d, gensym("outputs"), num_outputs);
    dictionary_appendlong(d, gensym("mute"), (flags & SV_MODULE_FLAG_MUTE) ? 1 : 0);
    dictionary_appendlong(d, gensym("solo"), (flags & SV_MODULE_FLAG_SOLO) ? 1 : 0);
    dictionary_appendlong(d, gensym("bypass"), (flags & SV_MODULE_FLAG_BYPASS) ? 1 : 0);

    t_atom av[1];
    atom_setobj(av, d);
    outlet_anything(x->info_outlet, gensym("module"), 1, av);
}


void svm_modules(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    int num_slots = sv_get_number_of_modules(0);

    // Output count first
    t_atom av[1];
    atom_setlong(av, num_slots);
    outlet_anything(x->info_outlet, gensym("modules_count"), 1, av);

    // Output each existing module
    for (int i = 0; i < num_slots; i++) {
        uint32_t flags = sv_get_module_flags(0, i);
        if (flags & SV_MODULE_FLAG_EXISTS) {
            const char *name = sv_get_module_name(0, i);
            const char *type = sv_get_module_type(0, i);
            t_atom mav[3];
            atom_setlong(mav, i);
            atom_setsym(mav + 1, gensym(type ? type : ""));
            atom_setsym(mav + 2, gensym(name ? name : ""));
            outlet_anything(x->info_outlet, gensym("module_slot"), 3, mav);
        }
    }
}


void svm_setmodulename(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: setmodulename <mod> <name>
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 2) {
        error("sunvox~: setmodulename requires module and name arguments");
        return;
    }

    int mod = svm_resolve_module(x, argv);
    if (mod < 0) return;

    const char *name = atom_getsym(argv + 1)->s_name;
    sv_set_module_name(0, mod, name);
}


void svm_setmodulexy(t_svm *x, t_atom_long mod, t_atom_long xpos, t_atom_long ypos)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_set_module_xy(0, (int)mod, (int)xpos, (int)ypos);
}


void svm_setmodulecolor(t_svm *x, t_atom_long mod, t_atom_long color)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_set_module_color(0, (int)mod, (int)color);
}


void svm_setfinetune(t_svm *x, t_atom_long mod, t_atom_long val)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_set_module_finetune(0, (int)mod, (int)val);
}


void svm_setrelnote(t_svm *x, t_atom_long mod, t_atom_long val)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    sv_set_module_relnote(0, (int)mod, (int)val);
}


//==============================================================================
// Controller Queries
//==============================================================================

void svm_getctl(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: getctl <mod> <ctl> [scaled]
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 2) {
        error("sunvox~: getctl requires module and controller arguments");
        return;
    }

    int mod = svm_resolve_module(x, argv);
    if (mod < 0) return;

    int ctl = (int)atom_getlong(argv + 1);
    int scaled = 0;
    if (argc > 2) {
        scaled = (int)atom_getlong(argv + 2);
    }

    int val = sv_get_module_ctl_value(0, mod, ctl, scaled);
    const char *name = sv_get_module_ctl_name(0, mod, ctl);
    int min = sv_get_module_ctl_min(0, mod, ctl, scaled);
    int max = sv_get_module_ctl_max(0, mod, ctl, scaled);
    int type = sv_get_module_ctl_type(0, mod, ctl);

    t_atom av[7];
    atom_setlong(av, mod);
    atom_setlong(av + 1, ctl);
    atom_setsym(av + 2, gensym(name ? name : ""));
    atom_setlong(av + 3, val);
    atom_setlong(av + 4, min);
    atom_setlong(av + 5, max);
    atom_setlong(av + 6, type);
    outlet_anything(x->info_outlet, gensym("ctl"), 7, av);
}


void svm_getctls(t_svm *x, t_atom_long mod)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    int num_ctls = sv_get_number_of_module_ctls(0, (int)mod);

    // Output count first
    t_atom cav[2];
    atom_setlong(cav, mod);
    atom_setlong(cav + 1, num_ctls);
    outlet_anything(x->info_outlet, gensym("ctls_count"), 2, cav);

    // Output each controller
    for (int i = 0; i < num_ctls; i++) {
        const char *name = sv_get_module_ctl_name(0, (int)mod, i);
        int val = sv_get_module_ctl_value(0, (int)mod, i, 0);
        int min = sv_get_module_ctl_min(0, (int)mod, i, 0);
        int max = sv_get_module_ctl_max(0, (int)mod, i, 0);
        int type = sv_get_module_ctl_type(0, (int)mod, i);

        t_atom av[7];
        atom_setlong(av, mod);
        atom_setlong(av + 1, i);
        atom_setsym(av + 2, gensym(name ? name : ""));
        atom_setlong(av + 3, val);
        atom_setlong(av + 4, min);
        atom_setlong(av + 5, max);
        atom_setlong(av + 6, type);
        outlet_anything(x->info_outlet, gensym("ctl"), 7, av);
    }
}


//==============================================================================
// Sampler/MetaModule
//==============================================================================

void svm_samplerload(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: samplerload <mod> <file> [sample_slot]
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 2) {
        error("sunvox~: samplerload requires module and filename arguments");
        return;
    }

    int mod = svm_resolve_module(x, argv);
    if (mod < 0) return;

    const char *filename = atom_getsym(argv + 1)->s_name;
    char filepath[MAX_PATH_CHARS];

    if (!svm_locate_file(x, filename, filepath)) {
        error("sunvox~: cannot find file %s", filename);
        return;
    }

    int sample_slot = -1;
    if (argc > 2) {
        sample_slot = (int)atom_getlong(argv + 2);
    }

    int res = sv_sampler_load(0, mod, filepath, sample_slot);
    if (res != 0) {
        error("sunvox~: failed to load sample into module %d", mod);
    }
}


void svm_metamoduleload(t_svm *x, t_atom_long mod, t_symbol *s)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    char filepath[MAX_PATH_CHARS];
    if (!svm_locate_file(x, s->s_name, filepath)) {
        error("sunvox~: cannot find file %s", s->s_name);
        return;
    }

    int res = sv_metamodule_load(0, (int)mod, filepath);
    if (res != 0) {
        error("sunvox~: failed to load metamodule content into module %ld", mod);
    }
}


void svm_vplayerload(t_svm *x, t_atom_long mod, t_symbol *s)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    char filepath[MAX_PATH_CHARS];
    if (!svm_locate_file(x, s->s_name, filepath)) {
        error("sunvox~: cannot find file %s", s->s_name);
        return;
    }

    int res = sv_vplayer_load(0, (int)mod, filepath);
    if (res != 0) {
        error("sunvox~: failed to load vorbis file into module %ld", mod);
    }
}


//==============================================================================
// Pattern Management
//==============================================================================

void svm_newpattern(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: newpattern <clone> <x> <y> <tracks> <lines> <name>
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 6) {
        error("sunvox~: newpattern requires: clone x y tracks lines name");
        return;
    }

    int clone = (int)atom_getlong(argv);
    int xpos = (int)atom_getlong(argv + 1);
    int ypos = (int)atom_getlong(argv + 2);
    int tracks = (int)atom_getlong(argv + 3);
    int lines = (int)atom_getlong(argv + 4);
    const char *name = atom_getsym(argv + 5)->s_name;

    sv_lock_slot(0);
    int pat = sv_new_pattern(0, clone, xpos, ypos, tracks, lines, 0, name);
    sv_unlock_slot(0);

    if (pat >= 0) {
        t_atom av[1];
        atom_setlong(av, pat);
        outlet_anything(x->info_outlet, gensym("newpattern"), 1, av);
    } else {
        error("sunvox~: failed to create pattern");
    }
}


void svm_removepattern(t_svm *x, t_atom_long pat)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    sv_lock_slot(0);
    int res = sv_remove_pattern(0, (int)pat);
    sv_unlock_slot(0);

    if (res != 0) {
        error("sunvox~: failed to remove pattern %ld", pat);
    }
}


void svm_findpattern(t_svm *x, t_symbol *s)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    int pat = sv_find_pattern(0, s->s_name);
    t_atom av[1];
    atom_setlong(av, pat);
    outlet_anything(x->info_outlet, gensym("findpattern"), 1, av);
}


void svm_getpattern(t_svm *x, t_atom_long pat)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    int lines = sv_get_pattern_lines(0, (int)pat);
    if (lines <= 0) {
        error("sunvox~: pattern %ld does not exist", pat);
        return;
    }

    const char *name = sv_get_pattern_name(0, (int)pat);
    int xpos = sv_get_pattern_x(0, (int)pat);
    int ypos = sv_get_pattern_y(0, (int)pat);
    int tracks = sv_get_pattern_tracks(0, (int)pat);

    t_dictionary *d = dictionary_new();
    dictionary_appendlong(d, gensym("id"), pat);
    dictionary_appendstring(d, gensym("name"), name ? name : "");
    dictionary_appendlong(d, gensym("x"), xpos);
    dictionary_appendlong(d, gensym("y"), ypos);
    dictionary_appendlong(d, gensym("tracks"), tracks);
    dictionary_appendlong(d, gensym("lines"), lines);

    t_atom av[1];
    atom_setobj(av, d);
    outlet_anything(x->info_outlet, gensym("pattern"), 1, av);
}


void svm_patterns(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    int num_slots = sv_get_number_of_patterns(0);

    // Output count first
    t_atom av[1];
    atom_setlong(av, num_slots);
    outlet_anything(x->info_outlet, gensym("patterns_count"), 1, av);

    // Output each existing pattern
    for (int i = 0; i < num_slots; i++) {
        int lines = sv_get_pattern_lines(0, i);
        if (lines > 0) {
            const char *name = sv_get_pattern_name(0, i);
            int tracks = sv_get_pattern_tracks(0, i);
            t_atom pav[4];
            atom_setlong(pav, i);
            atom_setsym(pav + 1, gensym(name ? name : ""));
            atom_setlong(pav + 2, tracks);
            atom_setlong(pav + 3, lines);
            outlet_anything(x->info_outlet, gensym("pattern_slot"), 4, pav);
        }
    }
}


void svm_setpatternxy(t_svm *x, t_atom_long pat, t_atom_long xpos, t_atom_long ypos)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    sv_lock_slot(0);
    sv_set_pattern_xy(0, (int)pat, (int)xpos, (int)ypos);
    sv_unlock_slot(0);
}


void svm_setpatternsize(t_svm *x, t_atom_long pat, t_atom_long tracks, t_atom_long lines)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    sv_lock_slot(0);
    sv_set_pattern_size(0, (int)pat, (int)tracks, (int)lines);
    sv_unlock_slot(0);
}


void svm_setpatternname(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: setpatternname <pat> <name>
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 2) {
        error("sunvox~: setpatternname requires pattern and name arguments");
        return;
    }

    int pat = (int)atom_getlong(argv);
    const char *name = atom_getsym(argv + 1)->s_name;

    sv_lock_slot(0);
    sv_set_pattern_name(0, pat, name);
    sv_unlock_slot(0);
}


void svm_patternmute(t_svm *x, t_atom_long pat, t_atom_long mute)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }

    sv_lock_slot(0);
    sv_pattern_mute(0, (int)pat, (int)mute);
    sv_unlock_slot(0);
}


void svm_setpatternevent(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: setpatternevent <pat> <track> <line> <nn> <vv> <mm> <ccee> <xxyy>
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 8) {
        error("sunvox~: setpatternevent requires 8 arguments: pat track line nn vv mm ccee xxyy");
        return;
    }

    int pat = (int)atom_getlong(argv);
    int track = (int)atom_getlong(argv + 1);
    int line = (int)atom_getlong(argv + 2);
    int nn = (int)atom_getlong(argv + 3);
    int vv = (int)atom_getlong(argv + 4);
    int mm = (int)atom_getlong(argv + 5);
    int ccee = (int)atom_getlong(argv + 6);
    int xxyy = (int)atom_getlong(argv + 7);

    int res = sv_set_pattern_event(0, pat, track, line, nn, vv, mm, ccee, xxyy);
    if (res != 0) {
        error("sunvox~: failed to set pattern event");
    }
}


void svm_getpatternevent(t_svm *x, t_symbol *s, long argc, t_atom *argv)
{
    // Usage: getpatternevent <pat> <track> <line>
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    if (argc < 3) {
        error("sunvox~: getpatternevent requires pat track line arguments");
        return;
    }

    int pat = (int)atom_getlong(argv);
    int track = (int)atom_getlong(argv + 1);
    int line = (int)atom_getlong(argv + 2);

    int nn = sv_get_pattern_event(0, pat, track, line, 0);
    int vv = sv_get_pattern_event(0, pat, track, line, 1);
    int mm = sv_get_pattern_event(0, pat, track, line, 2);
    int ccee = sv_get_pattern_event(0, pat, track, line, 3);
    int xxyy = sv_get_pattern_event(0, pat, track, line, 4);

    t_atom av[8];
    atom_setlong(av, pat);
    atom_setlong(av + 1, track);
    atom_setlong(av + 2, line);
    atom_setlong(av + 3, nn);
    atom_setlong(av + 4, vv);
    atom_setlong(av + 5, mm);
    atom_setlong(av + 6, ccee);
    atom_setlong(av + 7, xxyy);
    outlet_anything(x->info_outlet, gensym("patternevent"), 8, av);
}


//==============================================================================
// Utility
//==============================================================================

void svm_log(t_svm *x, t_atom_long size)
{
    if (size <= 0) size = 1024;
    const char *log = sv_get_log((int)size);
    if (log && log[0]) {
        post("sunvox~ log:\n%s", log);
    }
}


void svm_samplerate(t_svm *x)
{
    if (!x->is_initialized) {
        error("sunvox~: not initialized");
        return;
    }
    t_atom av[1];
    atom_setlong(av, sv_get_sample_rate());
    outlet_anything(x->info_outlet, gensym("samplerate"), 1, av);
}


void svm_version(t_svm *x)
{
    t_atom av[2];
    atom_setsym(av, gensym("sunvox~"));
    atom_setsym(av + 1, gensym("1.0.0"));
    outlet_anything(x->info_outlet, gensym("version"), 2, av);
}


//==============================================================================
// DSP
//==============================================================================

void svm_dsp64(t_svm *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	if (x->is_initialized) {
		post("sunvox~: reinitializing...");
        sv_close_slot(0);
        sv_deinit();
    }

    delete[] x->in_svm_buffer;
    delete[] x->out_svm_buffer;

    x->buffer_size = maxvectorsize;
    x->in_svm_buffer = new float[maxvectorsize * N_IN_CHANNELS];
    x->out_svm_buffer = new float[maxvectorsize * N_OUT_CHANNELS];

    memset(x->in_svm_buffer, 0, sizeof(float) * maxvectorsize * N_IN_CHANNELS);
    memset(x->out_svm_buffer, 0, sizeof(float) * maxvectorsize * N_OUT_CHANNELS);

    int ver = sv_init(0, samplerate, N_OUT_CHANNELS, SV_INIT_FLAG_USER_AUDIO_CALLBACK
                                                    | SV_INIT_FLAG_AUDIO_FLOAT32
                                                    | SV_INIT_FLAG_ONE_THREAD);
    if (ver >= 0) {
    	x->is_initialized = 1;
        sv_open_slot(0);
        // Apply stored attribute values
        sv_volume(0, (int)x->volume);
        sv_set_autostop(0, (int)x->autostop);
        post("sunvox~: initialized (SunVox version %d.%d.%d)",
             (ver >> 16) & 0xFF, (ver >> 8) & 0xFF, ver & 0xFF);
    } else {
    	error("sunvox~: initialization failed!");
    }
    object_method(dsp64, gensym("dsp_add64"), x, svm_perform64, 0, NULL);
}


void svm_perform64(t_svm *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts,
                           long sampleframes, long flags, void *userparam)
{
    float *in_ptr = x->in_svm_buffer;
    float *out_ptr = x->out_svm_buffer;
    int n = sampleframes;

    // Clear output buffer before processing
    memset(out_ptr, 0, sizeof(float) * n * N_OUT_CHANNELS);

    // Interleave input: Max provides separate channel buffers, SunVox expects interleaved
    // Buffer format: L0, R0, L1, R1, L2, R2, ...
    for (int i = 0; i < n; i++) {
        for (int chan = 0; chan < N_IN_CHANNELS; chan++) {
            if (chan < numins) {
                in_ptr[i * N_IN_CHANNELS + chan] = (float)ins[chan][i];
            } else {
                in_ptr[i * N_IN_CHANNELS + chan] = 0.0f;
            }
        }
    }

    // Process audio through SunVox
    sv_audio_callback2(out_ptr, n, LATENCY, sv_get_ticks(), FLOAT32_TYPE, N_IN_CHANNELS, in_ptr);

    // De-interleave output: SunVox provides interleaved, Max expects separate channel buffers
    for (int i = 0; i < n; i++) {
        for (int chan = 0; chan < N_OUT_CHANNELS; chan++) {
            if (chan < numouts) {
                outs[chan][i] = (double)out_ptr[i * N_OUT_CHANNELS + chan];
            }
        }
    }
}
