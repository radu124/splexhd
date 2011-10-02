#!/bin/bash
rm -rf tiles
mkdir -p tiles

if [ -z "$TILESIZE" ]; then TILESIZE=64; fi

convert tiles-${TILESIZE}.png -crop ${TILESIZE}x${TILESIZE} -set filename:tile "%[fx:page.y/${TILESIZE}]_%[fx:page.x/${TILESIZE}]" +repage +adjoin "tiles/t_%[filename:tile].png"
