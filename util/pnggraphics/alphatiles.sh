#!/bin/bash
rm -rf alpha
mkdir -p alpha

TILESIZEA=256



convert alpha-128.png -chop $[TILESIZEA*2]x0+$[TILESIZEA*3]+0 -crop ${TILESIZEA}x${TILESIZEA} -set filename:tile "%[fx:page.x/${TILESIZEA}+1]_%[fx:page.y/${TILESIZEA}+1]" +repage +adjoin "alpha/alpha_%[filename:tile].png"

TSAX=$[TILESIZEA/4]
TSAY=$[TILESIZEA/2]

convert alpha-128.png -chop $[TILESIZEA*3]x0+0+0 -crop ${TSAX}x${TSAY} -set filename:tile "%[fx:page.x/${TSAX}+1]_%[fx:page.y/${TSAY}+1]" +repage +adjoin "alpha/xalphadigit_%[filename:tile].png"

mv alpha/xalphadigit_1_1.png alpha/alphadigit_00.png
mv alpha/xalphadigit_2_1.png alpha/alphadigit_01.png
mv alpha/xalphadigit_3_1.png alpha/alphadigit_02.png
mv alpha/xalphadigit_4_1.png alpha/alphadigit_03.png
mv alpha/xalphadigit_5_1.png alpha/alphadigit_04.png

mv alpha/xalphadigit_1_2.png alpha/alphadigit_05.png
mv alpha/xalphadigit_2_2.png alpha/alphadigit_06.png
mv alpha/xalphadigit_3_2.png alpha/alphadigit_07.png
mv alpha/xalphadigit_4_2.png alpha/alphadigit_08.png
mv alpha/xalphadigit_5_2.png alpha/alphadigit_09.png

mv alpha/xalphadigit_6_1.png alpha/alphadigit_10.png

rm alpha/xalphadigit*

MONT1=`ls alpha/alphadigit_*.png | sort`

montage -tile 1x -geometry 64x128+0+0 -background transparent $MONT1 alpha/digits.png

convert alpha/alpha_1_1.png -chop 32x32+224+224 -scale 128x128 alpha/xalpha_1_1.png
convert alpha/alpha_2_1.png -chop 32x32+224+224 -scale 128x128 alpha/xalpha_2_1.png
convert alpha/alpha_3_1.png -chop 32x32+224+224 -scale 128x128 alpha/xalpha_3_1.png
convert alpha/digits.png -scale 50% alpha/xdigits.png

rm alpha/alphadigit*
rm alpha/alpha* alpha/digit*

rename "s:x::" alpha/x*

