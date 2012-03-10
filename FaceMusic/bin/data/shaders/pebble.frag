uniform sampler2DRect src_tex_unit0;


uniform float width;

uniform float mx;
uniform float my;

uniform float cx;
uniform float cy;
float angle;

void main( void ){  
    
    vec2 xy = vec2(gl_FragCoord.x - width, gl_FragCoord.y);
    vec2 xy_m_center = xy - vec2(cx,cy);
    
    float sn = sin(angle);  
    float cs = cos(angle);
    float px = xy_m_center.x * cs - xy_m_center.y * sn; 
    float py = xy_m_center.x * sn + xy_m_center.y * cs;
    
    vec2 newxy = vec2(px, py)*0.5 + vec2(mx,my);
    
	vec4 texel0 = texture2DRect(src_tex_unit0, newxy);
	
    float dist = distance(xy, vec2(cx, cy));
    
    // debugging if the center is right. 
    if (dist < 2.0){
        gl_FragColor =  vec4(1,1,1,1);
    } else {
        gl_FragColor =  gl_Color*texel0;
    }
    
   
    
//    vec2 xy = vec2(gl_FragCoord.x, gl_FragCoord.y);
//    
//    
//    
    
////    float px = xy.x * cs - xy.y * sn; 
////    float py = xy.x * sn + xy.y * cs;
////    vec2 newxy = vec2(px,py)  + vec2(100,100);
////    
//	//vec2 st = gl_TexCoord[0].st;
//	vec4 texel0 = texture2DRect(src_tex_unit0, xy);
//	
//    float dist = distance(vec2(mx, my), gl_FragCoord.xy);
//    if (dist < 50.0){
//        gl_FragColor =  vec4(1,1,1,1);
//    } else {
//        gl_FragColor =  gl_Color*texel0;
//    }
//    //texel0.r = 1.0;
//	//texel0.g = 1.0;
//	//texel0.b = 1.0;
	
    
}

