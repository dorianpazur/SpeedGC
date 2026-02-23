#!/bin/bash
gxtexconv -i tarmac_diffuse.png colfmt=14 mipmap=yes minlod=0 maxlod=4 -o tarmac_diffuse.tpl
gxtexconv -i tarmac_spec.png colfmt=0 mipmap=yes minlod=0 maxlod=4 width=256 height=1024

