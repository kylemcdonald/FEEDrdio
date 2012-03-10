uniform sampler2DRect src_tex_unit0;

uniform float pebbleRotation;
uniform float screenOffset;
uniform vec2 texCenter, pebbleCenter;

void main( void ) {
	vec2 screenPosition = vec2(gl_FragCoord.x - screenOffset, gl_FragCoord.y);
	vec2 local = screenPosition - pebbleCenter;
	float sn = sin(pebbleRotation), cs = cos(pebbleRotation);
	local = vec2(local.x * cs - local.y * sn, local.x * sn + local.y * cs);
	vec4 texel0 = texture2DRect(src_tex_unit0, texCenter + local);
	gl_FragColor =  gl_Color * texel0;
}

