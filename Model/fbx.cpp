//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Author:  Andi Liu
//


//#include "fbxsdk.h"
#include "fbx.h"
#include "Utility.h"

/* Tab character ("\t") counter */
int numTabs = 0;

/**
 * Print the required number of tabs.
 */
void FBX::PrintTabs() {
    for (int i = 0; i < numTabs; i++)
        Utility::Printf("\t");
}

/**
 * Return a string-based representation based on the attribute type.
 */
FbxString FBX::GetAttributeTypeName(FbxNodeAttribute::EType type) {
    switch (type) {
    case FbxNodeAttribute::eUnknown: return "unidentified";
    case FbxNodeAttribute::eNull: return "null";
    case FbxNodeAttribute::eMarker: return "marker";
    case FbxNodeAttribute::eSkeleton: return "skeleton";
    case FbxNodeAttribute::eMesh: return "mesh";
    case FbxNodeAttribute::eNurbs: return "nurbs";
    case FbxNodeAttribute::ePatch: return "patch";
    case FbxNodeAttribute::eCamera: return "camera";
    case FbxNodeAttribute::eCameraStereo: return "stereo";
    case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
    case FbxNodeAttribute::eLight: return "light";
    case FbxNodeAttribute::eOpticalReference: return "optical reference";
    case FbxNodeAttribute::eOpticalMarker: return "marker";
    case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
    case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
    case FbxNodeAttribute::eBoundary: return "boundary";
    case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
    case FbxNodeAttribute::eShape: return "shape";
    case FbxNodeAttribute::eLODGroup: return "lodgroup";
    case FbxNodeAttribute::eSubDiv: return "subdiv";
    default: return "unknown";
    }
}

void FBX::PrintMesh(FbxMesh* mesh)
{
    Utility::Printf("This node has a mesh! Here are its control points:\n");
    for (int i = 0; i < mesh->GetControlPointsCount(); i++) // Mesh->GetControlPoints, probably would be easier
    {
        FbxVector4 controlPoint; // I wonder why the control point has four coordinates. The fourth one always seems to be zero though.
        controlPoint = mesh->GetControlPointAt(i);
        Utility::Printf("Control point = (%f, %f, %f, %f)\n",
            controlPoint[0], controlPoint[1], controlPoint[2], controlPoint[3]
        );
    }
    Utility::Printf("That is all. Here are its polygon indices:\n");
    for (int i = 0; i < mesh->GetPolygonCount(); i++)
    {
        for (int j = 0; j < mesh->GetPolygonSize(i); j++)
        {
            Utility::Printf("%d,", mesh->GetPolygonVertex(i, j));
        }
        Utility::Printf("\n");
    }
    Utility::Printf("That is all.\n");
}

/**
 * Print an attribute.
 */
void FBX::PrintAttribute(FbxNodeAttribute* pAttribute) {
    if (!pAttribute) return;

    FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    FbxString attrName = pAttribute->GetName();
    PrintTabs();
    // Note: to retrieve the character array of a FbxString, use its Buffer() method.
    Utility::Printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
    if (typeName == "mesh")
    {
        FbxMesh* mesh = static_cast<FbxMesh*>(pAttribute); // Dynamic cast doesn't work here, so there might be something wrong
        PrintMesh(mesh);
    }

}

void FBX::Print(FbxVector4 vec, FbxString message)
{
    Utility::Printf("[%s(%f, %f, %f, %f)]\n", message, vec[0], vec[1], vec[2], vec[3]);
}

/**
 * Print a node, its attributes, and all its children recursively.
 */
void FBX::PrintNode(FbxNode* pNode) {
    PrintTabs();
    const char* nodeName = pNode->GetName();
    FbxDouble3 translation = pNode->LclTranslation.Get();
    FbxDouble3 rotation = pNode->LclRotation.Get();
    FbxDouble3 scaling = pNode->LclScaling.Get();

    // Print the contents of the node.
    Utility::Printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
        nodeName,
        translation[0], translation[1], translation[2],
        rotation[0], rotation[1], rotation[2],
        scaling[0], scaling[1], scaling[2]
    );
    numTabs++;

    // Print the node's attributes.
    for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
        PrintAttribute(pNode->GetNodeAttributeByIndex(i));

    // Recursively print the children.
    for (int j = 0; j < pNode->GetChildCount(); j++)
        PrintNode(pNode->GetChild(j));

    numTabs--;
    PrintTabs();
    Utility::Printf("</node>\n");
}

/**
 * Main function - loads the hard-coded fbx file,
 * and prints its contents in an xml format to stdout.
 */
int FBX::PrintFBXFile(std::wstring filePath) {

    // Change the following filename to a suitable filename value.
    std::string s(filePath.begin(), filePath.end());
    const char* lFilename = s.c_str();

    // Initialize the SDK manager. This object handles all our memory management.
    FbxManager* lSdkManager = FbxManager::Create();

    // Create the IO settings object.
    FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    // Create an importer using the SDK manager.
    FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

    // Use the first argument as the filename for the importer.
    if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
        Utility::Printf("Call to FbxImporter::Initialize() failed.\n");
        Utility::Printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
        exit(-1);
    }

    // Create a new scene so that it can be populated by the imported file.
    FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

    // Import the contents of the file into the scene.
    lImporter->Import(lScene);

    // The file is imported; so get rid of the importer.
    lImporter->Destroy();

    FbxVector4 boxMin, boxMax, boxCenter;
    lScene->ComputeBoundingBoxMinMaxCenter(boxMin, boxMax, boxCenter);
    Print(boxMin, "BoxMin: ");
    Print(boxMax, "BoxMax: ");
    Print(boxCenter, "BoxCenter: ");

    // Print the nodes of the scene and their attributes recursively.
    // Note that we are not printing the root node because it should
    // not contain any attributes.
    FbxNode* lRootNode = lScene->GetRootNode();
    if (lRootNode) {
        for (int i = 0; i < lRootNode->GetChildCount(); i++)
            PrintNode(lRootNode->GetChild(i));
    }

    // Destroy the SDK manager and all the other objects it was handling.
    lSdkManager->Destroy();
    return 0;
}

void FBX::BuildMaterials(ModelData& model, const Asset& asset)
{
    Utility::Printf("MATERIALS:\n");
    return;
    int numMaterials = 0; // Will change to actual material number
    for (int i = 0; i < numMaterials; i++)
    {
        MaterialConstantData& material = model.m_MaterialConstants[i];
        for (int j = 0; j < 4; j++)
            material.baseColorFactor[j] = 0.8;
        for (int j = 0; j < 3; j++)
            material.emissiveFactor[j] = 0;
        // Will add other factors. I hope default values should do?
    }
    // Will add textures
}

uint32_t FBX::WalkGraph(
    std::vector<GraphNode>& sceneGraph,
    BoundingSphere& modelBSphere,
    AxisAlignedBox& modelBBox,
    std::vector<Mesh*>& meshList,
    std::vector<byte>& bufferMemory,
    //const std::vector<glTF::Node*>& siblings,
    uint32_t curPos,
    const Matrix4& xform
) 
{
    return 0;
    size_t numSiblings = 1; // Will change to actual size

    for (size_t i = 0; i < numSiblings; ++i)
    {
        //glTF::Node* curNode = siblings[i];
        GraphNode& thisGraphNode = sceneGraph[curPos];
        thisGraphNode.hasChildren = 0;
        thisGraphNode.hasSibling = 0;
        thisGraphNode.matrixIdx = curPos;
        thisGraphNode.skeletonRoot = 0;
        //curNode->linearIdx = curPos;

        // They might not be used, but we have space to hold the neutral values which could be
        // useful when updating the matrix via animation.
        //std::memcpy((float*)&thisGraphNode.scale, curNode->scale, sizeof(curNode->scale));
        //std::memcpy((float*)&thisGraphNode.rotation, curNode->rotation, sizeof(curNode->rotation));

        thisGraphNode.scale = { 1.000000, 1.000000, 1.000000 };
        thisGraphNode.rotation = Quaternion(); // Quaternion??

        if (/*curNode->hasMatrix*/ false)
        {
            //std::memcpy((float*)&thisGraphNode.xform, curNode->matrix, sizeof(curNode->matrix)); // what is the x form?
        }
        else
        {
            thisGraphNode.xform = Matrix4(
                Matrix3(thisGraphNode.rotation) * Matrix3::MakeScale(thisGraphNode.scale), // why would a 3*3 matrix + a 3 * vector make a Matrix 4?
                //Vector3(*(const XMFLOAT3*)curNode->translation)
                Vector3()
            );
        }

        const Matrix4 LocalXform = xform * thisGraphNode.xform;

        //if (!curNode->pointsToCamera && curNode->mesh != nullptr)
        //{
        //    BoundingSphere sphereOS;
        //    AxisAlignedBox boxOS;
        //    CompileMesh(meshList, bufferMemory, *curNode->mesh, curPos, LocalXform, sphereOS, boxOS);
        //    modelBSphere = modelBSphere.Union(sphereOS);
        //    modelBBox.AddBoundingBox(boxOS);
        //}

        uint32_t nextPos = curPos + 1;

        //if (curNode->children.size() > 0)
        //{
        //    thisGraphNode.hasChildren = 1;
        //    nextPos = WalkGraph(sceneGraph, modelBSphere, modelBBox, meshList, bufferMemory, curNode->children, nextPos, LocalXform);
        //}

        // Are there more siblings?
        if (i + 1 < numSiblings)
        {
            thisGraphNode.hasSibling = 1;
        }

        curPos = nextPos;
    }

    return curPos;
}

//uint32_t FBX::WalkGraph(
//    std::vector<GraphNode>& sceneGraph,
//    std::vector<Mesh*>& meshList,
//    std::vector<byte>& bufferMemory,
//    bool hasSibling,
//    uint32_t curPos,
//    const Matrix4& xform
//)
//{
//    //glTF::Node* curNode = siblings[i];
//    GraphNode thisGraphNode = sceneGraph[curPos];
//    thisGraphNode.hasChildren = 0;
//    thisGraphNode.hasSibling = hasSibling;
//    thisGraphNode.matrixIdx = curPos;
//    thisGraphNode.skeletonRoot = 0;
//    //curNode->linearIdx = curPos;
//
//    // They might not be used, but we have space to hold the neutral values which could be
//    // useful when updating the matrix via animation.
//    //std::memcpy((float*)&thisGraphNode.scale, curNode->scale, sizeof(curNode->scale));
//    //std::memcpy((float*)&thisGraphNode.rotation, curNode->rotation, sizeof(curNode->rotation));
//
//    thisGraphNode.scale = { 1.000000, 1.000000, 1.000000 };
//    thisGraphNode.rotation = Quaternion(); // Quaternion??
//
//    if (/*curNode->hasMatrix*/ false)
//    {
//        //std::memcpy((float*)&thisGraphNode.xform, curNode->matrix, sizeof(curNode->matrix)); // what is the x form?
//    }
//    else
//    {
//        thisGraphNode.xform = Matrix4(
//            Matrix3(thisGraphNode.rotation) * Matrix3::MakeScale(thisGraphNode.scale), // why would a 3*3 matrix + a 3 * vector make a Matrix 4?
//            //Vector3(*(const XMFLOAT3*)curNode->translation)
//            Vector3()
//        );
//    }
//
//    const Matrix4 LocalXform = xform * thisGraphNode.xform;
//
//    //if (!curNode->pointsToCamera && curNode->mesh != nullptr)
//    //{
//    //    BoundingSphere sphereOS;
//    //    AxisAlignedBox boxOS;
//    //    CompileMesh(meshList, bufferMemory, *curNode->mesh, curPos, LocalXform, sphereOS, boxOS);
//    //    modelBSphere = modelBSphere.Union(sphereOS);
//    //    modelBBox.AddBoundingBox(boxOS);
//    //}
//
//    uint32_t nextPos = curPos + 1;
//
//    //if (curNode->children.size() > 0)
//    //{
//    //    thisGraphNode.hasChildren = 1;
//    //    nextPos = WalkGraph(sceneGraph, modelBSphere, modelBBox, meshList, bufferMemory, curNode->children, nextPos, LocalXform);
//    //}
//
//    // Are there more siblings?
//    if (i + 1 < numSiblings)
//    {
//        thisGraphNode.hasSibling = 1;
//    }
//
//    curPos = nextPos;
//
//    return curPos;
//}

void FBX::BuildAnimations(ModelData& model, const Asset& asset)
{
    Utility::Printf("ANIMATIONS:\n");
    // No animations yet
}

void FBX::BuildSkins(ModelData& model, const Asset& asset)
{
    Utility::Printf("SKINS:\n");
    // No skins yet
}

bool FBX::BuildModel(ModelData& model, const Asset& asset, int sceneIdx)
{
    Utility::Printf("----------------------------------------------------------------- \n");
    Utility::Printf("BUILD MODEL: \n");
    BuildMaterials(model, asset);

    std::vector<byte>& bufferMemory = model.m_GeometryData; // Store in a unified buffer... Why?

    model.m_BoundingSphere = BoundingSphere(kZero);
    model.m_BoundingBox = AxisAlignedBox(kZero);

    uint32_t numNodes = WalkGraph(model.m_SceneGraph, model.m_BoundingSphere, model.m_BoundingBox, model.m_Meshes, bufferMemory, /*scene->nodes,*/ 0, Matrix4(kIdentity));
    model.m_SceneGraph.resize(numNodes);



    BuildAnimations(model, asset);
    BuildSkins(model, asset);
    return false;
}

bool FBX::SaveModel(const std::wstring& filePath, const ModelData& data)
{
    Utility::Printf("SAVE MODEL:\n");
    return false;
}