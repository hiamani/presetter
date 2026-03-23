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
        "rect": [ 58.0, 100.0, 1000.0, 780.0 ],
        "showrootpatcherontab": 0,
        "showontab": 0,
        "boxes": [
            {
                "box": {
                    "id": "obj-28",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
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
                        "rect": [ 58.0, 126.0, 1000.0, 754.0 ],
                        "showontab": 2,
                        "boxes": [
                            {
                                "box": {
                                    "bubble": 1,
                                    "id": "obj-29",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 668.0, 99.5, 148.0, 37.0 ],
                                    "text": "Add, rename, and remove filters",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "id": "obj-28",
                                    "linecount": 4,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 479.0, 174.0, 187.0, 64.0 ],
                                    "text": "Add filter slots (if 'hello' doesn't exist, presetter automatically creates a filter),\nor clear filters of all slots",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "id": "obj-24",
                                    "linecount": 3,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 443.0, 257.5, 185.0, 51.0 ],
                                    "text": "Apply and reset filters to filter out presets. Multiple filters can be applied at once!",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "id": "obj-23",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 154.0, 333.0, 169.0, 24.0 ],
                                    "text": "Write filters to a JSON file",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-26",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 80.0, 334.0, 63.0, 22.0 ],
                                    "text": "writefilters"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-22",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 549.0, 107.0, 100.0, 22.0 ],
                                    "text": "removefilter hello"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-19",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 80.0, 107.0, 80.0, 22.0 ],
                                    "text": "addfilter hello"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-14",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 368.0, 107.0, 158.0, 22.0 ],
                                    "text": "renamefilter helloworld hello"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-27",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 1017.0, 222.0, 129.0, 22.0 ],
                                    "text": "removefilterslot hello 1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-25",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 368.0, 195.0, 86.0, 22.0 ],
                                    "text": "clearfilter hello"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-7",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 340.0, 272.0, 64.0, 22.0 ],
                                    "text": "resetfilters"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-18",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 211.0, 272.0, 87.0, 22.0 ],
                                    "text": "resetfilter hello"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-17",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 80.0, 272.0, 89.0, 22.0 ],
                                    "text": "applyfilter hello"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-8",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 189.0, 107.0, 158.0, 22.0 ],
                                    "text": "renamefilter hello helloworld"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-9",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 217.0, 195.0, 109.0, 22.0 ],
                                    "text": "addfilterslot hello 2"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-10",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 80.0, 195.0, 109.0, 22.0 ],
                                    "text": "addfilterslot hello 1"
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "bubbleside": 2,
                                    "id": "obj-15",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 505.0, 380.0, 152.0, 39.0 ],
                                    "text": "Our pattr objects!",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-2",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 3,
                                    "outlettype": [ "", "", "" ],
                                    "patching_rect": [ 618.0, 456.0, 40.0, 22.0 ],
                                    "restore": [ "This is Preset 1's text." ],
                                    "saved_object_attributes": {
                                        "parameter_enable": 0,
                                        "parameter_mappable": 0
                                    },
                                    "text": "pattr",
                                    "varname": "u476008155[2]"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-1",
                                    "linecount": 2,
                                    "maxclass": "textedit",
                                    "numinlets": 1,
                                    "numoutlets": 4,
                                    "outlettype": [ "", "int", "", "" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 618.0, 503.0, 100.0, 50.0 ],
                                    "text": "This is Preset 1's text.",
                                    "varname": "textedit"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-21",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "bang" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 533.0, 503.0, 50.0, 22.0 ],
                                    "varname": "number[1]"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-20",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "bang" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 443.0, 503.0, 50.0, 22.0 ],
                                    "varname": "number"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-12",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 3,
                                    "outlettype": [ "", "", "" ],
                                    "patching_rect": [ 533.0, 456.0, 40.0, 22.0 ],
                                    "restore": [ 24.0 ],
                                    "saved_object_attributes": {
                                        "parameter_enable": 0,
                                        "parameter_mappable": 0
                                    },
                                    "text": "pattr",
                                    "varname": "u476008155[1]"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-11",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 3,
                                    "outlettype": [ "", "", "" ],
                                    "patching_rect": [ 443.0, 456.0, 40.0, 22.0 ],
                                    "restore": [ 19.0 ],
                                    "saved_object_attributes": {
                                        "parameter_enable": 0,
                                        "parameter_mappable": 0
                                    },
                                    "text": "pattr",
                                    "varname": "u476008155"
                                }
                            },
                            {
                                "box": {
                                    "autowrite": 0,
                                    "defaulttab": 1,
                                    "filename": "filters2.json",
                                    "id": "obj-5",
                                    "maxclass": "presetter",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 37.0, 415.0, 289.0, 170.0 ],
                                    "pattrstorage": "repo"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-3",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 37.0, 59.0, 100.0, 22.0 ],
                                    "saved_object_attributes": {
                                        "client_rect": [ 1121, 382, 1623, 682 ],
                                        "parameter_enable": 0,
                                        "parameter_mappable": 0,
                                        "storage_rect": [ 583, 69, 1034, 197 ]
                                    },
                                    "text": "pattrstorage repo",
                                    "varname": "repo"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-13",
                                    "maxclass": "panel",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 428.0, 427.0, 306.0, 146.0 ]
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 89.5, 249.0, 46.5, 249.0 ],
                                    "source": [ "obj-10", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-20", 0 ],
                                    "midpoints": [ 463.0, 488.0, 452.5, 488.0 ],
                                    "source": [ "obj-11", 1 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-21", 0 ],
                                    "midpoints": [ 553.0, 488.0, 542.5, 488.0 ],
                                    "source": [ "obj-12", 1 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 377.5, 165.0, 46.5, 165.0 ],
                                    "source": [ "obj-14", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 89.5, 319.9453125, 46.5, 319.9453125 ],
                                    "source": [ "obj-17", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 220.5, 320.296875, 46.5, 320.296875 ],
                                    "source": [ "obj-18", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 89.5, 163.609375, 46.5, 163.609375 ],
                                    "source": [ "obj-19", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-1", 0 ],
                                    "midpoints": [ 638.0, 488.0, 627.5, 488.0 ],
                                    "source": [ "obj-2", 1 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 558.5, 165.0, 46.5, 165.0 ],
                                    "source": [ "obj-22", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 377.5, 249.625, 46.5, 249.625 ],
                                    "source": [ "obj-25", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 89.5, 388.5390625, 46.5, 388.5390625 ],
                                    "source": [ "obj-26", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "source": [ "obj-3", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 349.5, 320.30859375, 46.5, 320.30859375 ],
                                    "source": [ "obj-7", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 198.5, 164.34375, 46.5, 164.34375 ],
                                    "source": [ "obj-8", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "midpoints": [ 226.5, 249.1171875, 46.5, 249.1171875 ],
                                    "source": [ "obj-9", 0 ]
                                }
                            }
                        ]
                    },
                    "patching_rect": [ 107.0, 38.0, 47.0, 22.0 ],
                    "text": "p filters",
                    "varname": "help1[1]"
                }
            },
            {
                "box": {
                    "id": "obj-27",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
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
                        "rect": [ 0.0, 26.0, 1000.0, 754.0 ],
                        "showontab": 2,
                        "boxes": [
                            {
                                "box": {
                                    "bubble": 1,
                                    "id": "obj-18",
                                    "linecount": 3,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 425.0, 348.0, 250.0, 51.0 ],
                                    "text": "Clicking a cell selects a stored preset, shift-clicking stores a preset,\nshift-option/alt-clicking delets a preset",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "bubblepoint": 1.0,
                                    "bubbleside": 2,
                                    "id": "obj-14",
                                    "linecount": 8,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 21.0, 307.0, 107.0, 133.0 ],
                                    "text": "Status bar shows preset names on hover– or the last operation if not & a confirmation dialog when storing and deleting presets",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "bubblepoint": 0.0,
                                    "bubbleside": 2,
                                    "id": "obj-10",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 384.0, 251.0, 117.0, 39.0 ],
                                    "text": "Rename presets",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "bubblepoint": 0.0,
                                    "bubbleside": 0,
                                    "id": "obj-6",
                                    "linecount": 3,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 339.0, 470.0, 129.0, 66.0 ],
                                    "text": "Select between presets and filters tabs",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "id": "obj-4",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 425.0, 418.0, 104.0, 37.0 ],
                                    "text": "Page through presets",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "bubblepoint": 0.0,
                                    "bubbleside": 2,
                                    "id": "obj-22",
                                    "linecount": 3,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 145.0, 224.0, 171.0, 66.0 ],
                                    "text": "This connection is required to receive messages from pattrstorage!",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "bubblepoint": 0.0,
                                    "bubbleside": 2,
                                    "id": "obj-17",
                                    "linecount": 3,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 141.0, 45.0, 165.0, 66.0 ],
                                    "text": "Route the operation output to autosave pattrstorage for example",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "bubblepoint": 0.0,
                                    "bubbleside": 0,
                                    "id": "obj-16",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 141.0, 484.0, 165.0, 52.0 ],
                                    "text": "Bangs when a preset is stored, renamed, or deleted.",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "bubble": 1,
                                    "bubbleside": 2,
                                    "id": "obj-15",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 708.0, 45.0, 152.0, 39.0 ],
                                    "text": "Our pattr objects!",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-9",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "bang" ],
                                    "patching_rect": [ 339.0, 76.0, 58.0, 22.0 ],
                                    "text": "loadbang"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-7",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 339.0, 119.0, 47.0, 22.0 ],
                                    "text": "recall 1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-2",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 3,
                                    "outlettype": [ "", "", "" ],
                                    "patching_rect": [ 821.0, 121.0, 40.0, 22.0 ],
                                    "restore": [ "This is Preset 1's text." ],
                                    "saved_object_attributes": {
                                        "parameter_enable": 0,
                                        "parameter_mappable": 0
                                    },
                                    "text": "pattr",
                                    "varname": "u476008155[2]"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-1",
                                    "linecount": 2,
                                    "maxclass": "textedit",
                                    "numinlets": 1,
                                    "numoutlets": 4,
                                    "outlettype": [ "", "int", "", "" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 821.0, 168.0, 100.0, 50.0 ],
                                    "text": "This is Preset 1's text.",
                                    "varname": "textedit"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-21",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "bang" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 736.0, 168.0, 50.0, 22.0 ],
                                    "varname": "number[1]"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-20",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "bang" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 646.0, 168.0, 50.0, 22.0 ],
                                    "varname": "number"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-12",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 3,
                                    "outlettype": [ "", "", "" ],
                                    "patching_rect": [ 736.0, 121.0, 40.0, 22.0 ],
                                    "restore": [ 24.0 ],
                                    "saved_object_attributes": {
                                        "parameter_enable": 0,
                                        "parameter_mappable": 0
                                    },
                                    "text": "pattr",
                                    "varname": "u476008155[1]"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-11",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 3,
                                    "outlettype": [ "", "", "" ],
                                    "patching_rect": [ 646.0, 121.0, 40.0, 22.0 ],
                                    "restore": [ 19.0 ],
                                    "saved_object_attributes": {
                                        "parameter_enable": 0,
                                        "parameter_mappable": 0
                                    },
                                    "text": "pattr",
                                    "varname": "u476008155"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-8",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 129.0, 119.0, 109.0, 22.0 ],
                                    "text": "writejson repo.json"
                                }
                            },
                            {
                                "box": {
                                    "autowrite": 1,
                                    "defaulttab": 0,
                                    "filename": "filters.json",
                                    "id": "obj-5",
                                    "maxclass": "presetter",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 129.0, 297.0, 289.0, 170.0 ],
                                    "pattrstorage": "repo"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-3",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 129.0, 167.0, 100.0, 22.0 ],
                                    "saved_object_attributes": {
                                        "client_rect": [ 1121, 382, 1623, 682 ],
                                        "parameter_enable": 0,
                                        "parameter_mappable": 0,
                                        "storage_rect": [ 583, 69, 1034, 197 ]
                                    },
                                    "text": "pattrstorage repo",
                                    "varname": "repo"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-13",
                                    "maxclass": "panel",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 631.0, 92.0, 306.0, 146.0 ]
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [ "obj-20", 0 ],
                                    "midpoints": [ 666.0, 153.0, 655.5, 153.0 ],
                                    "source": [ "obj-11", 1 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-21", 0 ],
                                    "midpoints": [ 756.0, 153.0, 745.5, 153.0 ],
                                    "source": [ "obj-12", 1 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-1", 0 ],
                                    "midpoints": [ 841.0, 153.0, 830.5, 153.0 ],
                                    "source": [ "obj-2", 1 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-5", 0 ],
                                    "source": [ "obj-3", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-8", 0 ],
                                    "midpoints": [ 138.5, 480.2734375, 8.9765625, 480.2734375, 8.9765625, 97.578125, 138.5, 97.578125 ],
                                    "source": [ "obj-5", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-3", 0 ],
                                    "midpoints": [ 348.5, 154.0, 138.5, 154.0 ],
                                    "source": [ "obj-7", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-3", 0 ],
                                    "source": [ "obj-8", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-7", 0 ],
                                    "source": [ "obj-9", 0 ]
                                }
                            }
                        ]
                    },
                    "patching_rect": [ 33.0, 38.0, 47.0, 22.0 ],
                    "text": "p basic",
                    "varname": "help1"
                }
            }
        ],
        "lines": [],
        "autosave": 0
    }
}