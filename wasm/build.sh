#!/bin/sh
set -e
cd "$(dirname "$0")/.."

W="${W:-1920}"
H="${H:-1080}"

SRCS="libft/ft_putstr_fd.c libft/ft_strlen.c \
srcs/math/vec3.c srcs/math/vec3_bis.c srcs/math/vec3_ops.c srcs/math/quad.c \
srcs/parser/material.c \
srcs/render/camera.c srcs/render/render.c \
srcs/intersect/sphere.c srcs/intersect/plane.c \
srcs/intersect/cylinder.c srcs/intersect/cylinder_utils.c \
srcs/intersect/hit_object_bonus.c srcs/intersect/cone_bonus.c \
srcs/intersect/cone_utils_bonus.c srcs/intersect/triangle_bonus.c \
srcs/shading/color.c srcs/shading/diffuse.c srcs/shading/ambient.c \
srcs/shading/shadow.c srcs/shading/attenuation.c srcs/shading/shade_bonus.c \
srcs/shading/specular_bonus.c srcs/shading/checker_bonus.c \
wasm/scene_wonderland.c wasm/accel.c wasm/wasm_main.c"

INC="-I includes -I libft -I wasm/stub"
mkdir -p wasm/out

cc -O2 -DWIDTH=$W -DHEIGHT=$H $INC $SRCS wasm/native_check.c -lm \
	-o wasm/out/native_check

if [ "$1" = "native" ]; then
	exit 0
fi

docker run --rm -u "$(id -u):$(id -g)" -v "$(pwd):/src" -w /src emscripten/emsdk \
	emcc -O2 -DWIDTH=$W -DHEIGHT=$H $INC $SRCS \
	-sMODULARIZE=1 -sEXPORT_NAME=createMiniRT \
	-sEXPORTED_FUNCTIONS=_rt_init,_rt_set_cam,_rt_set_fov,_rt_render_band \
	-sEXPORTED_RUNTIME_METHODS=HEAPU8 \
	-sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web,worker,node \
	-o wasm/out/minirt.js

docker run --rm -u "$(id -u):$(id -g)" -v "$(pwd):/src" -w /src emscripten/emsdk \
	emcc -O2 -DWIDTH=1280 -DHEIGHT=720 $INC $SRCS \
	-sMODULARIZE=1 -sEXPORT_NAME=createMiniRT \
	-sEXPORTED_FUNCTIONS=_rt_init,_rt_set_cam,_rt_set_fov,_rt_render_band \
	-sEXPORTED_RUNTIME_METHODS=HEAPU8 \
	-sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web,worker,node \
	-o wasm/out/minirt720.js

docker run --rm -u "$(id -u):$(id -g)" -v "$(pwd)/wasm:/src" -w /src emscripten/emsdk \
	node verify.js wasm.rgba "$W" "$H"
mv wasm/wasm.rgba wasm/out/wasm.rgba 2>/dev/null || true

wasm/out/native_check wasm/out/native.rgba "$W" "$H"
if cmp -s wasm/out/native.rgba wasm/out/wasm.rgba; then
	echo "PARITY: wasm output byte identical to native"
else
	echo "PARITY: outputs differ"
fi
ls -la wasm/out/minirt.js wasm/out/minirt.wasm
