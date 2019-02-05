#version 330

out vec4 fragColor;

in vec2 texCoordVarying;

uniform sampler2DRect particles2;

float circle(in vec2 _st, in float _radius){
    vec2 dist = _st-vec2(0.5);
	return 1.-smoothstep(_radius-0.1,
                         _radius+0.05,
                         dot(dist,dist)*4.0);
}

void main()
{
	vec4 col = texture(particles2, texCoordVarying.st).xyzw;
    //	Circle shading
	col.a *= circle(gl_PointCoord, 0.45);
	col.a *= 0.87;

    fragColor = col;
}