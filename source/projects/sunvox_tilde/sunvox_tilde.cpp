/**
	@file
	sv~: a simple audio object for Max
	original by: jeremy bernstein, jeremy@bootsquad.com
	@ingroup examples
*/

#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects

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
	double			offset; 	// the value of a property of our object
	float *in_sv_buffer;     	// intermediate sunvox input buffer
    float *out_sv_buffer;    	// intermediate sunvox output buffer
} t_sv;


// method prototypes
void *sv_new(t_symbol *s, long argc, t_atom *argv);
void sv_free(t_sv *x);
void sv_assist(t_sv *x, void *b, long m, long a, char *s);
void sv_float(t_sv *x, double f);
t_max_err sv_test(t_sv *x);
// void sv_cleanup(t_sv *x);
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
	class_addmethod(c, (method)sv_test,		"test",				 0);
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


t_max_err sv_test(t_sv *x)
{
	// sys_getdspobjdspstate();
	//  void *dsp_setpostprocess(method pm);

    int ver = sv_init( 0, 48000, 2, SV_INIT_FLAG_USER_AUDIO_CALLBACK | SV_INIT_FLAG_AUDIO_FLOAT32);
    if( ver >= 0 )
    {
        sv_open_slot( 0 );
        /*
        The SunVox is initialized.
        Slot 0 is open and ready for use.
        Then you can load and play some files in this slot.
        */
        sv_close_slot( 0 );
        sv_deinit();
    }
    return MAX_ERR_NONE;
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

