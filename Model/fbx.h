//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Author:  Andi Liu
//

#include "fbxsdk.h"
#include "ModelLoader.h"
#pragma once

using namespace Renderer;

namespace FBX {
    int PrintFBXFile(std::wstring filePath);
    void PrintTabs();
    void PrintMesh(FbxMesh* mesh);
    void Print(FbxVector4 vec, FbxString message = "");
    FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
    void PrintAttribute(FbxNodeAttribute* pAttribute);
    void PrintNode(FbxNode* pNode);

	class Asset {
	public:
		Asset(const std::wstring& filepath) { PrintFBXFile(filepath); }


	};

    //void CompileMesh(
    //    std::vector<Mesh*>& meshList,
    //    std::vector<byte>& bufferMemory,
    //    glTF::Mesh& srcMesh,
    //    uint32_t matrixIdx,
    //    const Matrix4& localToObject,
    //    BoundingSphere& boundingSphere,
    //    AxisAlignedBox& boundingBox
    //)
    //{

    //}


    static uint32_t WalkGraph(
        std::vector<GraphNode>& sceneGraph,
        BoundingSphere& modelBSphere,
        AxisAlignedBox& modelBBox,
        std::vector<Mesh*>& meshList,
        std::vector<byte>& bufferMemory,
        //const std::vector<glTF::Node*>& siblings,
        uint32_t curPos,
        const Matrix4& xform
    );
    //{
    //}

    //inline void CompileTexture(const std::wstring& basePath, const std::string& fileName, uint8_t flags)
    //{
    //    CompileTextureOnDemand(basePath + Utility::UTF8ToWideString(fileName), flags);
    //}

    //inline void SetTextureOptions(std::map<std::string, uint8_t>& optionsMap, glTF::Texture* texture, uint8_t options)
    //{
    //    if (texture && texture->source && optionsMap.find(texture->source->path) == optionsMap.end())
    //        optionsMap[texture->source->path] = options;
    //}

    void BuildMaterials(ModelData& model, const FBX::Asset& asset);

    void BuildAnimations(ModelData& model, const FBX::Asset& asset);

    void BuildSkins(ModelData& model, const FBX::Asset& asset);

    bool BuildModel(ModelData& model, const FBX::Asset& asset, int sceneIdx = -1);

    bool SaveModel(const std::wstring& filePath, const ModelData& data);

}