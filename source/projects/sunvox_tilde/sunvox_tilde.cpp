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
#include <unistd.h>

#define SUNVOX_STATIC_LIB
#include <sunvox.h>

#define N_IN_CHANNELS 2
#define N_OUT_CHANNELS 2
#define FLOAT32_TYPE 2
#define LATENCY 0

// struct to represent the object's state
typedef struct _sv {
	t_pxobject		ob;			// the object itself (t_pxobject in MSP instead of t_object)
	int is_initialized;			// flag to indicate if sv_init has been successfully called
    int keep_running;           // flag to indicate whether to keep running or not
	double			offset; 	// the value of a property of our object
	float *in_sv_buffer;     	// intermediate sunvox input buffer
    float *out_sv_buffer;    	// intermediate sunvox output buffer
} t_sv;


// method prototypes
void *sv_new(t_symbol *s, long argc, t_atom *argv);
void sv_free(t_sv *x);
t_max_err sv_bang(t_sv *x);
t_max_err sv_test(t_sv *x);
void sv_assist(t_sv *x, void *b, long m, long a, char *s);
void sv_float(t_sv *x, double f);
void sv_dsp64(t_sv *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void sv_perform64(t_sv *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);


// global class pointer variable
static t_class *sv_class = NULL;


//***********************************************************************************************

void ext_main(void *r)
{
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.

	t_class *c = class_new("sunvox~", (method)sv_new, (method)dsp_free, (long)sizeof(t_sv), 0L, A_GIMME, 0);

	class_addmethod(c, (method)sv_float,	"float",	A_FLOAT, 0);
	class_addmethod(c, (method)sv_bang,		"bang",				 0);
    class_addmethod(c, (method)sv_test,     "test",              0);
	class_addmethod(c, (method)sv_dsp64,	"dsp64",	A_CANT,  0);
	class_addmethod(c, (method)sv_assist,	"assist",	A_CANT,  0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	sv_class = c;
}


void *sv_new(t_symbol *s, long argc, t_atom *argv)
{
	t_sv *x = (t_sv *)object_alloc(sv_class);

	if (x) {
		dsp_setup((t_pxobject *)x, 1);	// MSP inlets: arg is # of inlets and is REQUIRED!
		// use 0 if you don't need inlets
		outlet_new(x, "signal"); 		// signal outlet (note "signal" rather than NULL)
		x->offset = 0.0;
		x->is_initialized = 0;
        x->keep_running = 1;        
        x->in_sv_buffer = NULL;
        x->out_sv_buffer = NULL;		
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


void sv_float(t_sv *x, double f)
{
	x->offset = f;
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

    int ver = sv_init( 0, 48000, 2, SV_INIT_FLAG_USER_AUDIO_CALLBACK | SV_INIT_FLAG_AUDIO_FLOAT32);
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
    int n = sampleframes; // n = 64

    if (ins) {
        for (int i = 0; i < n; i++) {
            for (int chan = 0; chan < numins; chan++) {
                *(in_ptr++) = ins[chan][i];
            }
        }
    }

	// int sv_audio_callback2( void* buf, int frames, int latency, uint32_t out_time, int in_type, int in_channels, void* in_buf ) SUNVOX_FN_ATTR;
	sv_audio_callback2(x->out_sv_buffer, n, LATENCY, sv_get_ticks(), FLOAT32_TYPE, n, x->in_sv_buffer );

    for (int i = 0; i < n; i++) {
        for (int chan = 0; chan < numouts; chan++) {
            outs[chan][i] = *out_ptr++;
        }
    }
}

void* load_file(const char* name, size_t* file_size)
{
    void* rv = 0;
    FILE* f = fopen(name, "rb");
    if (f) {
        fseek(f, 0, 2);
        size_t size = ftell(f); // get file size
        rewind(f);
        post("file %s size: %d bytes", name, (int)size);
        if (size > 0) {
            rv = malloc(size);
            if (rv) {
                fread(rv, 1, size, f);
                if (file_size)
                    *file_size = size;
            }
        }
        fclose(f);
    }
    return rv;
}

t_max_err sv_bang(t_sv *x)
{
    if (x->keep_running) {
        x->keep_running = 0;
    } else {
        x->keep_running = 1;
    }
    return MAX_ERR_NONE;
}


t_max_err sv_test(t_sv *x)
{
    // signal(SIGINT, int_handler);

    post("Loading SunVox project file...");
    int res = -1;
    res = sv_load(0, "/Users/sa/Downloads/song01.sunvox");
    if (res == 0)
        post("Loaded.");
    else
        error("Load error %d.", res);

    // Set volume to 100%
    sv_volume(0, 256);

    post("Project name: %s", sv_get_song_name(0));
    int mm = sv_get_number_of_modules(0);
    post("Number of modules: %d", mm);
    for (int i = 0; i < mm; i++) {
        uint32_t flags = sv_get_module_flags(0, i);
        if ((flags & SV_MODULE_FLAG_EXISTS) == 0)
            continue;
        int input_slots = (flags & SV_MODULE_INPUTS_MASK) >> SV_MODULE_INPUTS_OFF;
        int output_slots = (flags & SV_MODULE_OUTPUTS_MASK) >> SV_MODULE_OUTPUTS_OFF;
        int* inputs = sv_get_module_inputs(0, i);
        int* outputs = sv_get_module_outputs(0, i);
        int number_of_inputs = 0;
        int number_of_outputs = 0;
        uint32_t xy = sv_get_module_xy(0, i);
        uint32_t ft = sv_get_module_finetune(0, i);
        int x, y, finetune, relnote;
        SV_GET_MODULE_XY(xy, x, y);
        SV_GET_MODULE_FINETUNE(ft, finetune, relnote);
        post("module %d: %s (%s); x=%d y=%d finetune=%d rel.note=%d\n",
               i, sv_get_module_name(0, i), sv_get_module_type(0, i), x, y,
               finetune, relnote);
        post("  IO PORTS:\n");
        for (int s = 0; s < input_slots; s++) {
            if (inputs[s] >= 0) {
                post("  input from module %d\n", inputs[s]);
                number_of_inputs++;
            }
        }
        for (int s = 0; s < output_slots; s++) {
            if (outputs[s] >= 0) {
                post("  output to module %d\n", outputs[s]);
                number_of_outputs++;
            }
        }
        post("  input slots: %d; output slots: %d; N of inputs: %d; N "
               "of outputs: %d;\n",
               input_slots, output_slots, number_of_inputs,
               number_of_outputs);
        post("  controllers:\n");
        int cn = sv_get_number_of_module_ctls(0, i);
        for (int c = 0; c < cn; c++) {
            post("    %d.%s: %d / %d-%d (type %d)\n", c,
                   sv_get_module_ctl_name(0, i, c),
                   sv_get_module_ctl_value(0, i, c, 2),
                   sv_get_module_ctl_min(0, i, c, 2),
                   sv_get_module_ctl_max(0, i, c, 2),
                   sv_get_module_ctl_type(0, i, c));
        }
    }

    // Show information about the first pattern:
    // show_pattern(0, 0);

    // Send two events (Note ON) to the module "Kicker":
    int m = sv_find_module(0, "Kicker");
    sv_set_event_t(0, 1, 0);
    sv_send_event(0, 0, 64, 129, m + 1, 0,
                  0); // track 0; note 64; velocity 129 (max);
    sleep(1);

    /*
    //Play the exact frequency in Hz:
    //(but the actual frequency will depend the module and its parameters)
    m = sv_find_module( 0, "Generator" );
    sv_send_event( 0, 0, NOTECMD_SET_PITCH, 129, m+1, 0,
    SV_FREQUENCY_TO_PITCH( 440 ) ); //440 Hz sleep( 1 ); sv_send_event( 0,
    0, NOTECMD_NOTE_OFF, 0, 0, 0, 0 ); sleep( 1 );
    */

    sv_play_from_beginning(0);

    while (x->keep_running) {
        post("Line counter: %f Module 7 -> %s = %d\n",
               (float)sv_get_current_line2(0) / 32,
               sv_get_module_ctl_name(0, 7, 1),    // Get controller name
               sv_get_module_ctl_value(0, 7, 1, 2) // Get controller value
        );
        sleep(1);
    }

    sv_stop(0);
    // sv_close_slot(0);
    // sv_deinit();

    return MAX_ERR_NONE;
}
