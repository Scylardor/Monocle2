// Monocle Game Engine source files - Alexandre Baron


#ifdef MOE_OPENGL

#include "OpenGLShaderProgram.h"

namespace moe
{
	OpenGLShaderProgram::OpenGLShaderProgram(OpenGLShaderProgram&& other) noexcept
	{
		m_program = other.m_program;
		other.m_program = ms_nullProgram;
		m_blockBindingToBlockIdx = std::move(other.m_blockBindingToBlockIdx);
		m_blockMemberNameToOffset = std::move(other.m_blockMemberNameToOffset);
	}


	OpenGLShaderProgram& OpenGLShaderProgram::operator=(OpenGLShaderProgram&& rhs) noexcept
	{
		if (&rhs != this)
		{
			m_program = rhs.m_program;
			rhs.m_program = ms_nullProgram;
			m_blockBindingToBlockIdx = std::move(rhs.m_blockBindingToBlockIdx);
			m_blockMemberNameToOffset = std::move(rhs.m_blockMemberNameToOffset);
		}

		return *this;
	}


	void OpenGLShaderProgram::BuildUniformBlockAccessCache()
	{
		GLint numBlocks = 0;
		glGetProgramInterfaceiv(m_program, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
		const GLenum blockProperties[1] = { GL_NUM_ACTIVE_VARIABLES };
		const GLenum activeUnifProp[1] = { GL_ACTIVE_VARIABLES };
		const GLenum unifProperties[3] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

		m_blockBindingToBlockIdx.Reserve(numBlocks);

		for (int blockIx = 0; blockIx < numBlocks; ++blockIx)
		{
			// First get uniform block name
			GLint nameLen;
			glGetActiveUniformBlockiv(m_program, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
			std::string blockName;
			blockName.resize(nameLen);
			glGetActiveUniformBlockName(m_program, blockIx, nameLen, nullptr, &blockName[0]);
			blockName.pop_back(); // strip the '\0'

			// Then get block binding and index.
			const unsigned int blockIndex = glGetUniformBlockIndex(m_program, blockName.c_str());

			GLint blockBinding;
			glGetActiveUniformBlockiv(m_program, blockIx, GL_UNIFORM_BLOCK_BINDING, &blockBinding);

			m_blockBindingToBlockIdx.Insert({blockBinding, blockIndex});

			GLint numActiveUnifs = 0;
			glGetProgramResourceiv(m_program, GL_UNIFORM_BLOCK, blockIx, 1, blockProperties, 1, NULL, &numActiveUnifs);

			if (!numActiveUnifs)
				continue;

			std::vector<GLint> blockUnifs(numActiveUnifs);
			glGetProgramResourceiv(m_program, GL_UNIFORM_BLOCK, blockIx, 1, activeUnifProp, numActiveUnifs, NULL, &blockUnifs[0]);

			m_blockBindingToBlockIdx.Reserve(m_blockBindingToBlockIdx.Size() + numActiveUnifs);

			for (int unifIx = 0; unifIx < numActiveUnifs; ++unifIx)
			{
				GLint values[3];
				glGetProgramResourceiv(m_program, GL_UNIFORM, blockUnifs[unifIx], 3, unifProperties, 3, NULL, values);

				// Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
				// C++11 would let you use a std::string directly.
				std::vector<char> nameData(values[0]);
				glGetProgramResourceName(m_program, GL_UNIFORM, blockUnifs[unifIx], (GLsizei)nameData.size(), NULL, &nameData[0]);
				std::string memberName(nameData.begin(), nameData.end() - 1);
				const char* nameStr = memberName.c_str();
				unsigned int indices[1];
				GLint offsets[1];

				glGetUniformIndices(m_program, 1, &nameStr, indices);
				glGetActiveUniformsiv(m_program, 1, indices, GL_UNIFORM_OFFSET, offsets);

				std::string blockMemberName;
				blockMemberName.reserve(blockName.size() + memberName.size() + 1);
				blockMemberName += blockName;
				blockMemberName += '.';
				blockMemberName += memberName;

				m_blockMemberNameToOffset.Insert({ blockMemberName, offsets[0]});
			}
		}
	}


	int OpenGLShaderProgram::GetBlockMemberOffset(const std::string& memberVariableName) const
	{
		auto memberVarIt = m_blockMemberNameToOffset.Find(memberVariableName);
		if (memberVarIt == m_blockMemberNameToOffset.End())
		{
			// TODO: log error...
			return INT_MAX;
		}

		return memberVarIt->second;
	}
}

#endif // #ifdef MOE_OPENGL