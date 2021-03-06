#include "Material.h"
/*========================================================================================
|                                   RC-Engine (c) 2016                                   |
|                             Project: RC-Engine                                         |
|                             File: Material.cpp                                         |
|                             Author: Ruscris2                                           |
==========================================================================================*/

Material::Material()
{
	diffuseTexture = NULL;
	specularTexture = NULL;
	specularShininess = 32.0f;
	specularStrength = 1.0f;
}

Material::~Material()
{
	specularTexture = NULL;
	diffuseTexture = NULL;
}

void Material::SetDiffuseTexture(Texture * texture)
{
	diffuseTexture = texture;
}

void Material::SetSpecularTexture(Texture * texture)
{
	specularTexture = texture;
}

void Material::SetSpecularStrength(float value)
{
	specularStrength = value;
}

void Material::SetSpecularShininess(float value)
{
	specularShininess = value;
}

Texture * Material::GetDiffuseTexture()
{
	return diffuseTexture;
}

Texture * Material::GetSpecularTexture()
{
	return specularTexture;
}

float Material::GetSpecularShininess()
{
	return specularShininess;
}

float Material::GetSpecularStrength()
{
	return specularStrength;
}
