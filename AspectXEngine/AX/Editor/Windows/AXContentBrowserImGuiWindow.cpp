// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXContentBrowserImGuiWindow.h"
#include "AX/Utils/AXUtils.h"
#include "AX/Content/AXContent.h"
#include "AX/Core/AXApplication.h"

/**
* Constructor
*/
AXContentBrowserImGuiWindow::AXContentBrowserImGuiWindow()
	: AXParent< AXEditorImGuiWindow, AXEditorImGuiWindow >( "Content Browser" )
{
	AnalyseContentDirectory( );
}

/**
* Destructor
*/
AXContentBrowserImGuiWindow::~AXContentBrowserImGuiWindow()
{

}

/**
* Override to render the contents of this editor window
*/
void AXContentBrowserImGuiWindow::RenderContents( )
{
	RenderFolderTree( );
	
	if( mContentDirectoryNodeToDisplay )
	{
		ImGui::SameLine( );

		RenderFolderContentsPanel( *mContentDirectoryNodeToDisplay );
	}
}

/**
* Renders the folder tree part of the content browser
*/
void AXContentBrowserImGuiWindow::RenderFolderTree_Recursive( ContentFoldersTreeNode& node, ContentFoldersTreeNode*& currentlySelectedNode, bool& currentlySelectedNodeValidated )
{
	if( currentlySelectedNode == &node )
	{
		currentlySelectedNodeValidated = true;
	}

	bool isOpen( node.mNodeOpen );
	ImGui::SetNextTreeNodeOpen( node.mNodeOpen, ImGuiSetCond_Always );
	
	if( ImGui::Selectable( node.mName.c_str( ), currentlySelectedNode == &node ) )
	{
		currentlySelectedNodeValidated = true;
		currentlySelectedNode = &node;
	}

	if( ImGui::IsItemHovered() )
	{
		if( ImGui::IsMouseDoubleClicked( 0 ) )
		{
			node.mNodeOpen = !node.mNodeOpen;
		}
	}

	if( isOpen && !node.mChilden.empty() )
	{
		ImGui::Indent( );

		for( uint32_t i( 0 ); i < node.mChilden.size( ); ++i )
		{
			ImGui::PushID( i );

			RenderFolderTree_Recursive( node.mChilden[ i ], currentlySelectedNode, currentlySelectedNodeValidated );

			ImGui::PopID( );
		}

		ImGui::Unindent( );
	}
}

/**
* Renders the folder tree part of the content browser
*/
void AXContentBrowserImGuiWindow::RenderFolderTree( )
{
	ImGui::BeginChild( "left pane", ImVec2( 250, 0 ), true );

	bool selectedNodeValidated( false );

	RenderFolderTree_Recursive( mRootContentDirectoryNode, mContentDirectoryNodeToDisplay, selectedNodeValidated );
	
	if( !selectedNodeValidated )
	{
		mContentDirectoryNodeToDisplay = nullptr;
	}

	if( !mContentDirectoryNodeToDisplay )
	{
		mContentDirectoryNodeToDisplay = &mRootContentDirectoryNode;
	}

	ImGui::EndChild( );
}

/**
* Renders the right hand panel showing folder contents
*/
void AXContentBrowserImGuiWindow::RenderFolderContentsPanel( ContentFoldersTreeNode& folder )
{
	ImGui::BeginGroup( );

	ImGui::Text( folder.mDirectory.GetPath().c_str( ) );
	ImGui::Separator( );

	ImGui::BeginChild( "Folder Contents" );

	RenderFolderContensPanelAsColumns( folder );

	ImGui::EndChild( );

	ImGui::EndGroup( );
}

/**
* Renders the folders contents in columns mode
*/
void AXContentBrowserImGuiWindow::RenderFolderContensPanelAsColumns( ContentFoldersTreeNode& folder )
{
	float minWidthPerColumn = 250.0f;
	int numColumns = ( int )( ImGui::GetContentRegionAvailWidth( ) / minWidthPerColumn );

	if( numColumns <= 0 )
	{
		numColumns = 1;
	}

	ImGui::Columns( numColumns, NULL, false );

	static const AXDirectory::Item* selectedItem = nullptr;
	static const ContentFoldersTreeNode* selectedFolder = nullptr;

	// Folders first...
	for( ContentFoldersTreeNode& item : folder.mChilden )
	{
		if( ImGui::Selectable( item.mName.c_str( ), ( selectedFolder == &item ), ImGuiSelectableFlags_AllowDoubleClick ) )
		{
			selectedFolder = &item;
			selectedItem = nullptr;

			if( ImGui::IsMouseDoubleClicked( 0 ) )
			{
				mContentDirectoryNodeToDisplay = &item;

				// Need to ensure all directorys above this one are open in the tree or this will fail because it will not validate on
				// the next frame
				ContentFoldersTreeNode* nodeToSetOpen( mContentDirectoryNodeToDisplay );

				do 
				{
					nodeToSetOpen->mNodeOpen = true;
					nodeToSetOpen = nodeToSetOpen->mParent;

				} while ( nodeToSetOpen );
			}
		}

		ImGui::NextColumn( );
	}
	
	for( const AXDirectory::Item& item : folder.mDirectory.GetContents( ) )
	{
		if( item.mType == AXDirectory::Item::Type::File )
		{
			if( ImGui::Selectable( item.mName.c_str( ), ( selectedItem == &item ), ImGuiSelectableFlags_AllowDoubleClick ) )
			{
				selectedFolder = nullptr;
				selectedItem = &item;

				if( ImGui::IsMouseDoubleClicked( 0 ) )
				{
					if( AXContent* content = AXContent::GetFrom( AXApplication::Get( ) ) )
					{
						content->RequestAssetLoad( item.mName );
					}
				}
			}

			ImGui::NextColumn( );
		}
	}

	ImGui::Columns( 1 );
}

/**
* Looks through the content directory and pulls out all the info we need
*/
void AXContentBrowserImGuiWindow::AnalyseContentDirectory_Recursive( ContentFoldersTreeNode& node, const AXString& path )
{
	if( node.mDirectory.Open( path ) )
	{
		for( const AXDirectory::Item& item : node.mDirectory.GetContents( ) )
		{
			if( item.mType == AXDirectory::Item::Type::Directory )
			{
				node.mChilden.push_back( ContentFoldersTreeNode( ) );

				node.mChilden.back( ).mName = item.mName;
				node.mChilden.back( ).mParent = &node;

				AnalyseContentDirectory_Recursive( node.mChilden.back( ), AXUtils::FormatString( "%s/%s", path.c_str(), item.mName.c_str( ) ) );
			}
		}
	}
}

/**
* Looks through the content directory and pulls out all the info we need
*/
void AXContentBrowserImGuiWindow::AnalyseContentDirectory( )
{
	// todo, make this async

	mRootContentDirectoryNode.mChilden.clear( );
	
	if( AXContent* content = AXContent::GetFrom( AXApplication::Get( ) ) )
	{
		AXContent::Settings& contentSettings( content->GetSettings( ) );

		mRootContentDirectoryNode.mName = contentSettings.mContentRootDirectory.Val();
		AnalyseContentDirectory_Recursive( mRootContentDirectoryNode, contentSettings.mContentRootDirectory );
	}
}