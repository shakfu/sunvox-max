{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9,
            "minor": 1,
            "revision": 2,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [ 100.0, 100.0, 887.0, 697.0 ],
        "default_fontsize": 10.0,
        "default_fontname": "Verdana",
        "gridsize": [ 5.0, 5.0 ],
        "boxes": [
            {
                "box": {
                    "id": "obj-2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 244.5, 339.5, 391.0, 21.0 ],
                    "text": "pattern (dictionary)"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "fontsize": 14.0,
                    "id": "obj-title",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 10.0, 329.0, 24.0 ],
                    "text": "sunvox~ - SunVox modular synth engine"
                }
            },
            {
                "box": {
                    "id": "obj-subtitle",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 30.0, 400.0, 19.0 ],
                    "text": "Load and play SunVox projects, trigger notes, control modules in real-time"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 60.0, 120.0, 19.0 ],
                    "text": "-- File I/O --"
                }
            },
            {
                "box": {
                    "id": "obj-load",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 80.0, 115.0, 21.0 ],
                    "text": "load song01.sunvox"
                }
            },
            {
                "box": {
                    "id": "obj-save",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 105.0, 115.0, 21.0 ],
                    "text": "save output.sunvox"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 150.0, 60.0, 120.0, 19.0 ],
                    "text": "-- Playback --"
                }
            },
            {
                "box": {
                    "id": "obj-playfrom",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 150.0, 80.0, 53.0, 21.0 ],
                    "text": "playfrom"
                }
            },
            {
                "box": {
                    "id": "obj-play",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 205.0, 80.0, 32.0, 21.0 ],
                    "text": "play"
                }
            },
            {
                "box": {
                    "id": "obj-pause",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 242.0, 80.0, 38.0, 21.0 ],
                    "text": "pause"
                }
            },
            {
                "box": {
                    "id": "obj-resume",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 150.0, 105.0, 46.0, 21.0 ],
                    "text": "resume"
                }
            },
            {
                "box": {
                    "id": "obj-stop",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 200.0, 105.0, 32.0, 21.0 ],
                    "text": "stop"
                }
            },
            {
                "box": {
                    "id": "obj-rewind",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 237.0, 105.0, 55.0, 21.0 ],
                    "text": "rewind 0"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 310.0, 60.0, 120.0, 19.0 ],
                    "text": "-- Song Info --"
                }
            },
            {
                "box": {
                    "id": "obj-getinfo",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 310.0, 80.0, 43.0, 21.0 ],
                    "text": "getinfo"
                }
            },
            {
                "box": {
                    "id": "obj-getname",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 357.0, 80.0, 53.0, 21.0 ],
                    "text": "getname"
                }
            },
            {
                "box": {
                    "id": "obj-getbpm",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 412.0, 80.0, 47.0, 21.0 ],
                    "text": "getbpm"
                }
            },
            {
                "box": {
                    "id": "obj-getline",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 310.0, 105.0, 42.0, 21.0 ],
                    "text": "getline"
                }
            },
            {
                "box": {
                    "id": "obj-getlength",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 355.0, 105.0, 56.0, 21.0 ],
                    "text": "getlength"
                }
            },
            {
                "box": {
                    "id": "obj-endofsong",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 412.0, 105.0, 60.0, 21.0 ],
                    "text": "endofsong"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section4",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 485.0, 60.0, 120.0, 19.0 ],
                    "text": "-- Volume --"
                }
            },
            {
                "box": {
                    "id": "obj-vol0",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 485.0, 80.0, 29.5, 21.0 ],
                    "text": "0"
                }
            },
            {
                "box": {
                    "id": "obj-vol256",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 520.0, 80.0, 29.5, 21.0 ],
                    "text": "256"
                }
            },
            {
                "box": {
                    "id": "obj-dial",
                    "maxclass": "dial",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "float" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 555.0, 75.0, 35.0, 35.0 ],
                    "size": 256.0
                }
            },
            {
                "box": {
                    "id": "obj-volume",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 485.0, 115.0, 61.0, 21.0 ],
                    "text": "volume $1"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section5",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 610.0, 60.0, 120.0, 19.0 ],
                    "text": "-- Autostop --"
                }
            },
            {
                "box": {
                    "id": "obj-autostop-toggle",
                    "maxclass": "toggle",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 610.0, 80.0, 24.0, 24.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-autostop",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 610.0, 110.0, 69.0, 21.0 ],
                    "text": "autostop $1"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section6",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 145.0, 200.0, 19.0 ],
                    "text": "-- Notes & Events --"
                }
            },
            {
                "box": {
                    "id": "obj-note-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 165.0, 200.0, 19.0 ],
                    "text": "note <mod> <note> [vel] [track]"
                }
            },
            {
                "box": {
                    "id": "obj-note1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 185.0, 62.0, 21.0 ],
                    "text": "note 1 60"
                }
            },
            {
                "box": {
                    "id": "obj-note2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 87.0, 185.0, 62.0, 21.0 ],
                    "text": "note 1 64"
                }
            },
            {
                "box": {
                    "id": "obj-note3",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 154.0, 185.0, 62.0, 21.0 ],
                    "text": "note 1 67"
                }
            },
            {
                "box": {
                    "id": "obj-noteoff",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 210.0, 55.0, 21.0 ],
                    "text": "noteoff 1"
                }
            },
            {
                "box": {
                    "id": "obj-allnotesoff",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 80.0, 210.0, 65.0, 21.0 ],
                    "text": "allnotesoff"
                }
            },
            {
                "box": {
                    "id": "obj-cleansynths",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 150.0, 210.0, 68.0, 21.0 ],
                    "text": "cleansynths"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section7",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 240.0, 145.0, 200.0, 19.0 ],
                    "text": "-- Modules --"
                }
            },
            {
                "box": {
                    "id": "obj-modules",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 240.0, 165.0, 51.0, 21.0 ],
                    "text": "modules"
                }
            },
            {
                "box": {
                    "id": "obj-getmodule",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 295.0, 165.0, 72.0, 21.0 ],
                    "text": "getmodule 0"
                }
            },
            {
                "box": {
                    "id": "obj-findmodule",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 370.0, 165.0, 103.0, 21.0 ],
                    "text": "findmodule Output"
                }
            },
            {
                "box": {
                    "id": "obj-newmodule",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 240.0, 190.0, 165.0, 21.0 ],
                    "text": "newmodule Generator MyGen"
                }
            },
            {
                "box": {
                    "id": "obj-connect",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 240.0, 215.0, 70.0, 21.0 ],
                    "text": "connect 1 0"
                }
            },
            {
                "box": {
                    "id": "obj-removemodule",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 315.0, 215.0, 93.0, 21.0 ],
                    "text": "removemodule 1"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section8",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 485.0, 145.0, 200.0, 19.0 ],
                    "text": "-- Controllers --"
                }
            },
            {
                "box": {
                    "id": "obj-getctls",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 485.0, 165.0, 55.0, 21.0 ],
                    "text": "getctls 1"
                }
            },
            {
                "box": {
                    "id": "obj-getctl",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 545.0, 165.0, 60.0, 21.0 ],
                    "text": "getctl 1 0"
                }
            },
            {
                "box": {
                    "id": "obj-ctl-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 485.0, 190.0, 200.0, 19.0 ],
                    "text": "ctl <mod> <ctl> <val> [scaled]"
                }
            },
            {
                "box": {
                    "id": "obj-ctl",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 485.0, 210.0, 70.0, 21.0 ],
                    "text": "ctl 1 0 128"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section9",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 700.0, 145.0, 150.0, 19.0 ],
                    "text": "-- Patterns --"
                }
            },
            {
                "box": {
                    "id": "obj-patterns",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 700.0, 165.0, 50.0, 21.0 ],
                    "text": "patterns"
                }
            },
            {
                "box": {
                    "id": "obj-getpattern",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 755.0, 165.0, 71.0, 21.0 ],
                    "text": "getpattern 0"
                }
            },
            {
                "box": {
                    "id": "obj-patternmute",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 700.0, 190.0, 91.0, 21.0 ],
                    "text": "patternmute 0 1"
                }
            },
            {
                "box": {
                    "id": "obj-patternunmute",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 790.0, 190.0, 91.0, 21.0 ],
                    "text": "patternmute 0 0"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section10",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 700.0, 220.0, 150.0, 19.0 ],
                    "text": "-- Utility --"
                }
            },
            {
                "box": {
                    "id": "obj-version",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 700.0, 240.0, 45.0, 21.0 ],
                    "text": "version"
                }
            },
            {
                "box": {
                    "id": "obj-samplerate",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 750.0, 240.0, 64.0, 21.0 ],
                    "text": "samplerate"
                }
            },
            {
                "box": {
                    "id": "obj-log",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 815.0, 240.0, 26.0, 21.0 ],
                    "text": "log"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-sunvox-label",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 270.0, 209.0, 19.0 ],
                    "text": "sunvox~ @volume 256 @autostop 0"
                }
            },
            {
                "box": {
                    "fontname": "Verdana",
                    "fontsize": 10.0,
                    "id": "obj-sunvox",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 3,
                    "outlettype": [ "signal", "signal", "" ],
                    "patching_rect": [ 20.0, 290.0, 192.0, 21.0 ],
                    "saved_object_attributes": {
                        "volume": 218
                    },
                    "text": "sunvox~ @volume 256 @autostop 0"
                }
            },
            {
                "box": {
                    "id": "obj-outlet-comment1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 315.0, 80.0, 19.0 ],
                    "text": "Left signal"
                }
            },
            {
                "box": {
                    "id": "obj-outlet-comment2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 105.0, 315.0, 80.0, 19.0 ],
                    "text": "Right signal"
                }
            },
            {
                "box": {
                    "id": "obj-outlet-comment3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 185.0, 315.0, 80.0, 19.0 ],
                    "text": "Info outlet"
                }
            },
            {
                "box": {
                    "id": "obj-gain",
                    "maxclass": "gain~",
                    "multichannelvariant": 0,
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 340.0, 130.0, 20.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-dac",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 375.0, 45.0, 45.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-scope",
                    "maxclass": "scope~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 80.0, 375.0, 100.0, 50.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-meter-l",
                    "maxclass": "meter~",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "float" ],
                    "patching_rect": [ 190.0, 375.0, 80.0, 15.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-meter-r",
                    "maxclass": "meter~",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "float" ],
                    "patching_rect": [ 190.0, 395.0, 80.0, 15.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-loaded",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 185.0, 395.0, 90.0, 21.0 ],
                    "text": "loaded:"
                }
            },
            {
                "box": {
                    "id": "obj-dict-display",
                    "maxclass": "dict.view",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 290.0, 440.0, 300.0, 200.0 ]
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-keyboard-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 440.0, 275.0, 19.0 ],
                    "text": "-- MIDI Keyboard (requires module in slot 1) --"
                }
            },
            {
                "box": {
                    "id": "obj-kslider",
                    "maxclass": "kslider",
                    "numinlets": 2,
                    "numoutlets": 2,
                    "outlettype": [ "int", "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 460.0, 252.0, 53.0 ],
                    "range": 36
                }
            },
            {
                "box": {
                    "id": "obj-pack",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 520.0, 70.0, 21.0 ],
                    "text": "pack i i"
                }
            },
            {
                "box": {
                    "id": "obj-note-msg",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 545.0, 80.0, 21.0 ],
                    "text": "note 1 $1 $2"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-allnotesoff", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-autostop", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-autostop", 0 ],
                    "source": [ "obj-autostop-toggle", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-cleansynths", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-connect", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-ctl", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-volume", 0 ],
                    "source": [ "obj-dial", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-endofsong", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-findmodule", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dac", 1 ],
                    "order": 2,
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dac", 0 ],
                    "order": 3,
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-meter-l", 0 ],
                    "order": 0,
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-scope", 0 ],
                    "order": 1,
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getbpm", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getctl", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getctls", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getinfo", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getlength", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getline", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getmodule", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getname", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-getpattern", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-pack", 1 ],
                    "source": [ "obj-kslider", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-pack", 0 ],
                    "source": [ "obj-kslider", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-load", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-log", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-modules", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-newmodule", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "source": [ "obj-note-msg", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-note1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-note2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-note3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-noteoff", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-note-msg", 0 ],
                    "source": [ "obj-pack", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-patternmute", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-patterns", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-patternunmute", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-pause", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-play", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-playfrom", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-removemodule", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-resume", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-rewind", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-samplerate", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-save", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-stop", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 1 ],
                    "source": [ "obj-sunvox", 2 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-gain", 0 ],
                    "source": [ "obj-sunvox", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-meter-r", 0 ],
                    "source": [ "obj-sunvox", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-version", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-volume", 0 ],
                    "source": [ "obj-vol0", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-volume", 0 ],
                    "source": [ "obj-vol256", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-sunvox", 0 ],
                    "hidden": 1,
                    "source": [ "obj-volume", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}