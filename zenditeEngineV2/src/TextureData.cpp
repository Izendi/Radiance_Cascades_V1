#include "TextureData.h"

#include "vendor/stb_image/stb_image.h"

TextureData::TextureData(std::string typeName)
{
	glGenTextures(1, &texHandle);
	type = typeName;
}

std::string TextureData::getType() const
{
	return type;
}

void TextureData::setupTextureJPG(unsigned int unit, std::string texfilepath)
{
	this->filePath = texfilepath;
	stbi_set_flip_vertically_on_load(true);
	imgData = stbi_load(texfilepath.c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

	if (!imgData)
	{
		std::cout << "\n --- Failed to load texture --- \n";
		ASSERT(false);
	}

	//GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));

	changeTexUnit(unit);

	//Set default texture parameters.
	setTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_CLAMP_TO_EDGE
	setTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT); //GL_CLAMP_TO_EDGE
	setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData); // upload tex data to the GPU
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imgData);
}

void TextureData::setupTexturePNG(unsigned int unit, std::string texfilepath)
{
	this->filePath = texfilepath;
	texUnit = unit;
	changeTexUnit(unit);

	stbi_set_flip_vertically_on_load(true);
	imgData = stbi_load(texfilepath.c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

	if (!imgData)
	{
		std::cout << "\n --- Failed to load texture --- \n";
		ASSERT(false);
	}

	//GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));

	//changeTexUnit(unit);

	//Set default texture parameters.
	setTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	setTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_CLAMP_TO_EDGE);
	setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D); //#FIX_URGENT Throws Exceptions randomly.
	stbi_image_free(imgData);
}

void TextureData::setupHeightMapTexturePNG(unsigned int unit, const char* texfilepath)
{
	this->filePath = texfilepath;
	texUnit = unit;
	changeTexUnit(unit);

	stbi_set_flip_vertically_on_load(true);
	imgData = stbi_load(texfilepath, &imgWidth, &imgHeight, &nrChannels, 0);

	if (!imgData)
	{
		std::cout << "\n --- Failed to load texture --- \n";
		ASSERT(false);
	}

}

void TextureData::setupTexture(const char* texfilepath)
{
	this->filePath = texfilepath;
	stbi_set_flip_vertically_on_load(true);
	imgData = stbi_load(texfilepath, &imgWidth, &imgHeight, &nrChannels, 0);

	if (!imgData)
	{
		std::cout << "\n --- Failed to load texture --- \n";
		ASSERT(false);
	}

	std::cout << "NR Channels = " << nrChannels << std::endl;

	GLenum format = GL_RGBA;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	//GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));

	//changeTexUnit(unit);

	//Set default texture parameters.
	setTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	setTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imgData);
}

void TextureData::setTexParameteri(GLenum pname, GLint param) const
{
	GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	glTexParameteri(GL_TEXTURE_2D, pname, param);

}

void TextureData::changeTexUnit(unsigned int unit)
{

	texUnit = unit;

	if (unit == 0)
	{
		glActiveTexture(GL_TEXTURE0);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 1)
	{
		glActiveTexture(GL_TEXTURE1);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 2)
	{
		glActiveTexture(GL_TEXTURE2);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 3)
	{
		glActiveTexture(GL_TEXTURE3);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 4)
	{
		glActiveTexture(GL_TEXTURE4);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 5)
	{
		glActiveTexture(GL_TEXTURE5);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 6)
	{
		glActiveTexture(GL_TEXTURE6);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 7)
	{
		glActiveTexture(GL_TEXTURE7);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 8)
	{
		glActiveTexture(GL_TEXTURE8);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 9)
	{
		glActiveTexture(GL_TEXTURE9);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 10)
	{
		glActiveTexture(GL_TEXTURE10);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 11)
	{
		glActiveTexture(GL_TEXTURE11);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 12)
	{
		glActiveTexture(GL_TEXTURE12);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 13)
	{
		glActiveTexture(GL_TEXTURE13);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 14)
	{
		glActiveTexture(GL_TEXTURE14);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 15)
	{
		glActiveTexture(GL_TEXTURE15);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else if (unit == 16)
	{
		glActiveTexture(GL_TEXTURE16);
		GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	}
	else
	{
		std::cout << "\n --- Invalid texture unit value ---\n Must be between 0 - 16\n";
		ASSERT(false);
	}
}

void TextureData::bindTexture() const
{
	GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
}

unsigned int TextureData::getTexHandle() const
{
	//GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	return texHandle;
}

unsigned int TextureData::getTexUnit() const
{
	//GLCALL(glBindTexture(GL_TEXTURE_2D, texHandle));
	return texUnit;
}

std::string TextureData::getTexFilePath() const
{
	return filePath;
}
