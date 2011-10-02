#!/bin/bash
rm -rf sets
mkdir -p sets

TILESIZEA=128
TILESIZE=64

function tilenames()
{
	ROW=$1
	FIRST=$2
	LAST=$3
	for ((i=$2; i<$3; i++)); do echo -n "tiles/t_$1_$i.png "; done
}

montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 0 0 20; tilenames 1 0 20` tiles/t_19_12.png sets/tiles.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 2 0 20; tilenames 3 0 20` sets/murphfaces.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 4 0 20` sets/murphwalk.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 6 0 20` sets/tronroll.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 7 0 20` sets/zonkroll.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 8 0 10` sets/knikcut.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 10 0 10` sets/knikturn.png

montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 12 0 20` sets/explode.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 13 0 20` sets/explodeinfo.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 14 0 20` sets/elektron.png

montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 15 0 8` sets/vanishdisk.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 15 8 20` sets/vanishinfo.png
montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 16 0 8` sets/vanishpcb.png

montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 16 8 16` sets/console.png

montage -tile 1x -geometry ${TILESIZE}x${TILESIZE}+0+0 `tilenames 18 0 20` sets/bug.png
