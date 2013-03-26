

#version 130
precision highp float;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;

uniform sampler3D VolumeTexture;
uniform vec3 CameraPos;
uniform float invScale;
uniform float SampleDist;

in vec3 SurfacePos;
in vec3 WorldPos;

out vec4 Color;

vec4 Blend(vec4 A, vec4 B)
{
	return (1.0 - A.a) * A + B.a * B;
}

void main ()
{
	vec4 Result = vec4(0,0,0,0);
	
	vec4 Ray = vec4(normalize(WorldPos - CameraPos)*invScale*SampleDist,0);
	
	vec4 Coords = vec4(SurfacePos,0);
	
	
	
	while ((Coords.x >= 0 && Coords.x <= 1) &&
	       (Coords.y >= 0 && Coords.y <= 1) &&
		   (Coords.z >= 0 && Coords.z <= 1))
	{
		Result = Blend(Result,texture(VolumeTexture,Coords.xyz));
		Coords += Ray;
	}
	
	Color = Result + vec4(1,1,1,1);
}