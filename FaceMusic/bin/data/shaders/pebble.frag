uniform sampler2DRect src_tex_unit0;
uniform float rotation;
uniform vec2 texCenter;

varying vec2 position, center;

void main( void ) {
	vec2 local = (position.xy - center.xy);
	float sn = sin(rotation), cs = cos(rotation);
	local = vec2(local.x * cs - local.y * sn, local.x * sn + local.y * cs);
	vec4 texel0 = texture2DRect(src_tex_unit0, texCenter + local);
	gl_FragColor =  gl_Color * texel0;
}

