#version 330

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;
uniform sampler2DRect particles2;
uniform sampler2DRect particles3;

uniform vec3 mouse;
uniform vec2 resolution;
uniform float radiusSquared;
uniform float sense;
uniform float elapsed;

uniform vec3 rep1;
uniform vec3 rep2;
uniform vec3 rep3;

uniform float noise_scale;
uniform float noise_frequency;

in vec2 texCoordVarying;

layout(location = 0) out vec4 posOut;
layout(location = 1) out vec4 velOut;
layout(location = 2) out vec4 colOut;
layout(location = 3) out vec4 spdOut;

#define TWO_PI 6.283185307179586476925286766559

float atan2(in float y, in float x)
{
    bool s = (abs(x) > abs(y));
    return mix(TWO_PI/4.0 - atan(x,y), atan(y,x), s);
}

float rand(vec2 n) { 
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

vec3 hash33(vec3 p) { 

    // Faster, but doesn't disperse things quite as nicely as the block below it. However, when framerate
    // is an issue, and it often is, this is the one to use. Basically, it's a tweaked amalgamation I put
    // together, based on a couple of other random algorithms I've seen around... so use it with caution,
    // because I make a tonne of mistakes. :)
    float n = sin(dot(p, vec3(7, 157, 113)));    
    return fract(vec3(2097152, 262144, 32768)*n)*2. - 1.; // return fract(vec3(64, 8, 1)*32768.0*n)*2.-1.; 

    // I'll assume the following came from IQ.
    //p = vec3( dot(p, vec3(127.1, 311.7, 74.7)), dot(p, vec3(269.5, 183.3, 246.1)), dot(p, vec3(113.5, 271.9, 124.6)));
    //return (fract(sin(p)*43758.5453)*2. - 1.);

}

float noise(in vec3 p)
{
    // Skewing the cubic grid, then determining the first vertice and fractional position.
    vec3 i = floor(p + dot(p, vec3(0.333333)) );  p -= i - dot(i, vec3(0.166666)) ;
    
    // Breaking the skewed cube into tetrahedra with partitioning planes, then determining which side of the 
    // intersecting planes the skewed point is on. Ie: Determining which tetrahedron the point is in.
    vec3 i1 = step(p.yzx, p), i2 = max(i1, 1.0-i1.zxy); i1 = min(i1, 1.0-i1.zxy);    
    
    // Using the above to calculate the other three vertices. Now we have all four tetrahedral vertices.
    // Technically, these are the vectors from "p" to the vertices, but you know what I mean. :)
    vec3 p1 = p - i1 + 0.166666, p2 = p - i2 + 0.333333, p3 = p - 0.5;
  

    // 3D simplex falloff - based on the squared distance from the fractional position "p" within the 
    // tetrahedron to the four vertice points of the tetrahedron. 
    vec4 v = max(0.5 - vec4(dot(p,p), dot(p1,p1), dot(p2,p2), dot(p3,p3)), 0.0);
    
    // Dotting the fractional position with a random vector generated for each corner -in order to determine 
    // the weighted contribution distribution... Kind of. Just for the record, you can do a non-gradient, value 
    // version that works almost as well.
    vec4 d = vec4(dot(p, hash33(i)), dot(p1, hash33(i + i1)), dot(p2, hash33(i + i2)), dot(p3, hash33(i + 1.)));
    
     
    // Simplex noise... Not really, but close enough. :)
    return clamp(dot(d, v*v*v*8.)*1.732 + .5, 0., 1.); // Not sure if clamping is necessary. Might be overkill.

}
void main()
{
    vec3 pos = texture(particles0, texCoordVarying.st).xyz;
    vec3 vel = texture(particles1, texCoordVarying.st).xyz;
    vec4 col = texture(particles2, texCoordVarying.st).xyzw;

    float speed = texture(particles3, texCoordVarying.st).x;

    /*
    // mouse attraction
    vec3 direction = mouse - pos.xyz;
    float distSquared = dot(direction, direction);
    float magnitude = speed * (1.0 - distSquared / radiusSquared);
    vec3 force = step(distSquared, radiusSquared) * magnitude * normalize(direction);
    
    // gravity
    force += vec3(0.0, -0.5, 0.0);
    
    // accelerate
    vel += elapsed * force;
    
    // bounce off the sides
    //vel.x *= step(abs(pos.x), 0.5 * resolution.x) * 2.0 - 1.0;
    //vel.y *= step(abs(pos.y), 0.5 * resolution.y) * 2.0 - 1.0;
    
    // damping
    vel *= 0.995;
    
    // move
    pos += elapsed * vel;
    */

    float angle = noise(vec3(pos.x/noise_scale, pos.y/noise_scale, 1.0)) * TWO_PI * noise_frequency;
    vec2 dir = vec2(cos(angle), sin(angle));

    vel.xy = dir * speed;

    //  Repulsion
    float colorweight = col.b * col.b * col.b;
    colorweight = 0.1 * clamp(colorweight, 0.25, 1.0);
    vec3 rep = rep1 - pos.xyz;
    float distSquared = dot(rep, rep);
    float magnitude = speed * (1.0 - distSquared / radiusSquared);
    float amp = 6 + 1 * sin(elapsed);
    vec3 force = sense * amp * colorweight * step(distSquared, radiusSquared) * magnitude * normalize(rep);

    vel += elapsed * force;
/*
    rep = rep2 - pos.xyz;
    distSquared = dot(rep, rep);
    amp = 36 + 6 * sin(elapsed);
    magnitude = speed * (1.0 - distSquared / radiusSquared);
    force = sense * amp * colorweight * step(distSquared, radiusSquared) * magnitude * normalize(rep);

    vel += elapsed * force;

    rep = rep3 - pos.xyz;
    distSquared = dot(rep, rep);
    amp = 36 + 6 * sin(elapsed);
    magnitude = speed * (1.0 - distSquared / radiusSquared);
    force = sense * amp * colorweight * step(distSquared, radiusSquared) * magnitude * normalize(rep);

    vel += elapsed * force;
*/
    if(length(vel) > 80.0)
    {
        vel *=  55.0 / length(vel);
    }

    pos.x += vel.x;
    pos.y += vel.y;

    if(pos.x < -50 || pos.x > resolution.x + 50 || pos.y < -50 || pos.y > resolution.y + 50){
            pos.xy = vec2(resolution.x * rand(resolution.xy * elapsed), resolution.y * rand(resolution.yx * elapsed));
        }
    
    float specular = 2.0 * atan2(dir.x, dir.y) / TWO_PI;
    specular *= 1.0;
    
    colOut += specular;

    //  Velocity to alpha
    colOut.a = smoothstep(2.0, 50.0, length(vel));
    colOut.a = 0.0;
    
    posOut = vec4(pos, 1.0);
    velOut = vec4(vel, 0.0);
    colOut = vec4(col);
    spdOut = texture(particles3, texCoordVarying.st).xyzw;
}
