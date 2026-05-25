#pragma once

#include "Daydream/Graphics/Resources/Shader.h"

namespace Daydream
{
	class ShaderGroup
	{
	public:
		const Array<Shader*>& GetShaders() const;
		Shader* GetShader(ShaderType _type) const;

		static Shared<ShaderGroup> Create(const Path& _vertexShaderPath, const Path&
_pixelShaderPath);
		static Shared<ShaderGroup> CreateBuiltin(const Path& _vertexShaderName);
		static Shared<ShaderGroup> CreateBuiltin(const Path& _vertexShaderName, const Path&
			_pixelShaderName);


		static Shared<ShaderGroup> Create(Shader* _vertexShader);
		static Shared<ShaderGroup> Create(Shader* _vertexShader,
			Shader* _pixelShader);
		static Shared<ShaderGroup> Create(Shader* _vertexShader,
			Shader* _geometryShader,
			Shader* _pixelShader);
		static Shared<ShaderGroup> Create(Shader* _vertexShader,
			Shader* _hullShader,
			Shader* _domainShader,
			Shader* _geometryShader,
			Shader* _pixelShader);

		const Array<ShaderReflectionData>& GetInputData() const { return inputReflectionData; }
		const HashMap<String, ShaderReflectionData>& GetShaderBindingMap() const { return shaderBindingMap; }
		const HashMap<String, ShaderResourceType>& GetMaterialMap() const { return materialMap; }
		const ShaderReflectionData* GetShaderBindingInfo(const String& _name) const;

		UInt32 GetSetCount() const { return setCount; }
	private:
		ShaderGroup(Shader* _vertexShader, Shader* _hullShader, Shader* _domainShader, Shader* _geometryShader, Shader* _pixelShader);

		void CreateInputReflectionData();
		void CreateShaderBindingMap();
		void CreateMaterialMap();

		void ValidateInterface() { /* ... */ }
		void GenerateUnifiedReflection() { /* ... */ }

		UInt32 setCount = 0;

		Shader* vertexShader = nullptr;
		Shader* hullShader = nullptr; 
		Shader* domainShader = nullptr;
		Shader* geometryShader = nullptr;
		Shader* pixelShader = nullptr;

		Array<Shader*> shaders;

		Array<ShaderReflectionData> inputReflectionData;
		HashMap<String, ShaderReflectionData> shaderBindingMap;
		HashMap<String, ShaderResourceType> materialMap;
	};
}
