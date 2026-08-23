#!/bin/sh
set -e
cd "$(dirname "$0")/.."

SRCS="libft/ft_atoi.c libft/ft_bzero.c libft/ft_calloc.c libft/ft_isalnum.c \
libft/ft_isalpha.c libft/ft_isascii.c libft/ft_isdigit.c libft/ft_isprint.c \
libft/ft_isspace.c libft/ft_itoa.c libft/ft_memchr.c libft/ft_memcmp.c \
libft/ft_memcpy.c libft/ft_memmove.c libft/ft_memset.c libft/ft_putchar_fd.c \
libft/ft_putendl_fd.c libft/ft_putnbr_fd.c libft/ft_putstr_fd.c libft/ft_split.c \
libft/ft_strchr.c libft/ft_strcmp.c libft/ft_strdup.c libft/ft_striteri.c \
libft/ft_strjoin.c libft/ft_strlcat.c libft/ft_strlcpy.c libft/ft_strlen.c \
libft/ft_strmapi.c libft/ft_strncmp.c libft/ft_strnstr.c libft/ft_strrchr.c \
libft/ft_strtrim.c libft/ft_substr.c libft/ft_tolower.c libft/ft_toupper.c \
srcs/math/vec3.c srcs/math/vec3_bis.c srcs/math/vec3_ops.c srcs/math/vec3_rot.c \
srcs/math/quad.c \
srcs/parser/parse_scene.c srcs/parser/parse_ambient.c srcs/parser/parse_camera.c \
srcs/parser/material.c srcs/parser/parse_objects.c srcs/parser/parse_utils.c \
srcs/parser/tok_utils.c srcs/parser/parse_num.c srcs/parser/parse_num2.c \
srcs/parser/free_scene.c srcs/parser/dispatch_bonus.c srcs/parser/parse_opts_bonus.c \
srcs/parser/parse_light_bonus.c srcs/parser/parse_objects_bonus.c \
srcs/render/camera.c srcs/render/render.c srcs/render/preview.c \
srcs/render/translation.c srcs/render/rotation.c \
srcs/intersect/intersect.c srcs/intersect/sphere.c srcs/intersect/plane.c \
srcs/intersect/cylinder.c srcs/intersect/cylinder_utils.c \
srcs/intersect/hit_object_bonus.c srcs/intersect/cone_bonus.c \
srcs/intersect/cone_utils_bonus.c srcs/intersect/triangle_bonus.c \
srcs/shading/color.c srcs/shading/diffuse.c srcs/shading/ambient.c \
srcs/shading/shadow.c srcs/shading/attenuation.c srcs/shading/shade_bonus.c \
srcs/shading/specular_bonus.c srcs/shading/checker_bonus.c \
srcs/window/mouse_move.c \
wasm/wasm_main.c"

INC="-I includes -I libft -I wasm/stub"
mkdir -p wasm/out

cp scenes/bonus/showcase.rt wasm/out/scene.rt

cc -O2 -Wall -Wextra $INC $SRCS wasm/native_check.c -lm -o wasm/out/native_check
cd wasm/out && ./native_check native.rgba && cd ../..

docker run --rm -u "$(id -u):$(id -g)" -v "$(pwd):/src" -w /src emscripten/emsdk \
	emcc -O2 $INC $SRCS \
	-sMODULARIZE=1 -sEXPORT_NAME=createMiniRT \
	-sEXPORTED_FUNCTIONS=_rt_init,_rt_tick,_rt_idle,_rt_key,_rt_press,_rt_move,_rt_release,_rt_wheel \
	-sEXPORTED_RUNTIME_METHODS=HEAPU8 \
	-sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web,node \
	--embed-file wasm/out/scene.rt@scene.rt \
	-o wasm/out/minirt.js

docker run --rm -u "$(id -u):$(id -g)" -v "$(pwd)/wasm:/src" -w /src emscripten/emsdk \
	node verify.js wasm.rgba
mv wasm/wasm.rgba wasm/out/wasm.rgba 2>/dev/null || true

if cmp -s wasm/out/native.rgba wasm/out/wasm.rgba; then
	echo "PARITY: wasm output byte identical to native"
else
	python3 - <<'EOF'
a = open('wasm/out/native.rgba', 'rb').read()
b = open('wasm/out/wasm.rgba', 'rb').read()
d = sum(1 for i in range(0, len(a), 4) if a[i:i + 4] != b[i:i + 4])
print('PARITY: %d of %d pixels differ (%.4f%%)' % (d, len(a) // 4, 100.0 * d / (len(a) // 4)))
EOF
fi
ls -la wasm/out/minirt.js wasm/out/minirt.wasm
