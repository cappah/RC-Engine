#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_BONES 64

layout (binding = 0) uniform UBO
{
	mat4 mvp;
	mat4 worldMatrix;
	
} ubo;

layout (binding = 1) uniform BoneUniform
{
	mat4 bones[MAX_BONES];
} boneUniform;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormals;
layout (location = 3) in vec4 weights;
layout (location = 4) in ivec4 boneIDs;

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec2 outTexCoord;
layout (location = 2) out vec3 outNormals;

void main()
{
	mat4 boneTransform = boneUniform.bones[boneIDs[0]] * weights[0];
	boneTransform += boneUniform.bones[boneIDs[1]] * weights[1];
	boneTransform += boneUniform.bones[boneIDs[2]] * weights[2];
	boneTransform += boneUniform.bones[boneIDs[3]] * weights[3];

	vec4 tempPos = vec4(pos, 1.0f);
	gl_Position = ubo.mvp * boneTransform * tempPos;
	
	// outWorldPos
	outWorldPos = vec3(ubo.worldMatrix * boneTransform * tempPos);
	
	// outTexCoord
	outTexCoord = inTexCoord;
	
	// outNormals
	outNormals = mat3(transpose(inverse(ubo.worldMatrix))) * inNormals;
}