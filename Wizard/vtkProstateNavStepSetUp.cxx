/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkProstateNavStepSetUp.h"

#include "vtkProstateNavGUI.h"
#include "vtkProstateNavLogic.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWCheckButton.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWPushButton.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"

#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkOpenIGTLinkIFLogic.h"
#include "vtkOpenIGTLinkIFGUI.h"

#include "vtkMRMLRobotNode.h"
#include "vtkMRMLTransPerinealProstateRobotNode.h"
#include "vtkMRMLTransPerinealProstateSmartTemplateNode.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavStepSetUp);
vtkCxxRevisionMacro(vtkProstateNavStepSetUp, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkProstateNavStepSetUp::vtkProstateNavStepSetUp()
{

  //this->SetName("Configuration");
  this->SetTitle("Configuration");
  this->SetDescription("System configuration.");

  this->ConnectorFrame                  = NULL;
  this->RobotConnectorSelector          = NULL;
  this->ScannerConnectorSelector        = NULL;
  this->LogFileButton                   = NULL;

}


//----------------------------------------------------------------------------
vtkProstateNavStepSetUp::~vtkProstateNavStepSetUp()
{

  // Connectors
  if (this->ConnectorFrame)
    {
    this->ConnectorFrame->SetParent(NULL);
    this->ConnectorFrame->Delete();
    this->ConnectorFrame = NULL;
    }

  if (this->RobotConnectorSelector)
    {
    this->RobotConnectorSelector->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->RobotConnectorSelector->SetParent(NULL);
    this->RobotConnectorSelector->Delete();
    this->RobotConnectorSelector = NULL;
    }
  if (this->ScannerConnectorSelector)
    {
    this->ScannerConnectorSelector->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->ScannerConnectorSelector->SetParent(NULL);
    this->ScannerConnectorSelector->Delete();
    this->ScannerConnectorSelector = NULL;
    }
  if (this->LogFileButton)
    {
    this->LogFileButton->GetWidget()->GetLoadSaveDialog()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->LogFileButton->SetParent(NULL);
    this->LogFileButton->Delete();
    this->LogFileButton = NULL;
    }

}


//----------------------------------------------------------------------------
void vtkProstateNavStepSetUp::ShowUserInterface()
{

  this->Superclass::ShowUserInterface();
  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  vtkKWWidget *parent = wizardWidget->GetClientArea();

  // Connector Frame

  if (!this->ConnectorFrame)
    {
    this->ConnectorFrame = vtkKWFrame::New();
    this->ConnectorFrame->SetParent ( parent );
    this->ConnectorFrame->Create ( );
    }

  this->Script ( "pack %s -side top -fill x",
                 this->ConnectorFrame->GetWidgetName());

  if (!this->RobotConnectorSelector)
    {
    this->RobotConnectorSelector = vtkSlicerNodeSelectorWidget::New() ;
    this->RobotConnectorSelector->SetParent(this->ConnectorFrame);
    this->RobotConnectorSelector->Create();
    this->RobotConnectorSelector->SetNodeClass("vtkMRMLIGTLConnectorNode", NULL, NULL, "RobotConnector");
    this->RobotConnectorSelector->SetMRMLScene(this->MRMLScene);
    this->RobotConnectorSelector->SetBorderWidth(2);
    this->RobotConnectorSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff();
    this->RobotConnectorSelector->GetWidget()->GetWidget()->SetWidth(24);
    this->RobotConnectorSelector->SetNoneEnabled(1);
    this->RobotConnectorSelector->SetNewNodeEnabled(1);
    this->RobotConnectorSelector->SetLabelText( "Robot connector: ");
    this->RobotConnectorSelector->SetBalloonHelpString("Select or create a target list.");
    this->RobotConnectorSelector->ExpandWidgetOff();
    this->RobotConnectorSelector->SetLabelWidth(18);

    this->RobotConnectorSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    this->RobotConnectorSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NewNodeEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }    

  if (!this->ScannerConnectorSelector)
    {
    this->ScannerConnectorSelector = vtkSlicerNodeSelectorWidget::New() ;
    this->ScannerConnectorSelector->SetParent(this->ConnectorFrame);
    this->ScannerConnectorSelector->Create();
    this->ScannerConnectorSelector->SetNodeClass("vtkMRMLIGTLConnectorNode", NULL, NULL, "ScannerConnector");
    this->ScannerConnectorSelector->SetMRMLScene(this->MRMLScene);
    this->ScannerConnectorSelector->SetBorderWidth(2);
    this->ScannerConnectorSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff();
    this->ScannerConnectorSelector->GetWidget()->GetWidget()->SetWidth(24);
    this->ScannerConnectorSelector->SetNoneEnabled(1);
    this->ScannerConnectorSelector->SetNewNodeEnabled(1);
    this->ScannerConnectorSelector->SetLabelText( "Scanner Connector: ");
    this->ScannerConnectorSelector->SetBalloonHelpString("Select or create a target list.");
    this->ScannerConnectorSelector->ExpandWidgetOff();
    this->ScannerConnectorSelector->SetLabelWidth(18);

    this->ScannerConnectorSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    this->ScannerConnectorSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NewNodeEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }    

  this->Script("pack %s %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->RobotConnectorSelector->GetWidgetName(),
               this->ScannerConnectorSelector->GetWidgetName());

  if (!this->LogFileButton)
    {
    this->LogFileButton = vtkKWLoadSaveButtonWithLabel::New();
    this->LogFileButton->SetLabelText("Log file: ");
    this->LogFileButton->SetParent(this->ConnectorFrame);
    this->LogFileButton->Create();
    //this->LogFileButton->SetWidth(50);
    this->LogFileButton->GetWidget()->SetText ("Log file path");
    this->LogFileButton->GetWidget()->GetLoadSaveDialog()->SaveDialogOn();
    this->LogFileButton->GetWidget()->GetLoadSaveDialog()
      ->AddObserver( vtkKWFileBrowserDialog::FileNameChangedEvent,
                     (vtkCommand *)this->GUICallbackCommand );
    }
  this->Script("pack %s -side right -fill x -expand y -padx 2 -pady 2", 
               this->LogFileButton->GetWidgetName());
  
}


//----------------------------------------------------------------------------
void vtkProstateNavStepSetUp::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkProstateNavStepSetUp::ProcessGUIEvents( vtkObject *caller,
                                         unsigned long event, void *callData )
{

  vtkMRMLRobotNode* robotNode = NULL;

  if (this->ProstateNavManager)
    {
    robotNode = this->ProstateNavManager->GetRobotNode();
    }

  //vtkMRMLTransPerinealProstateRobotNode* robotNode=GetRobotNode();

  if (this->RobotConnectorSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && (event == vtkSlicerNodeSelectorWidget::NewNodeEvent || event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent))
    {
    vtkMRMLIGTLConnectorNode* node = vtkMRMLIGTLConnectorNode::SafeDownCast(this->RobotConnectorSelector->GetSelected());

    if (node)
      {
      vtkMRMLTransPerinealProstateRobotNode* transperinealRobotNode;
      vtkMRMLTransPerinealProstateSmartTemplateNode* transperinealSmartTemplateNode;

      if (transperinealRobotNode = 
          vtkMRMLTransPerinealProstateRobotNode::SafeDownCast(robotNode))
        {      
        vtksys_stl::string moduleShareDir=this->GetLogic()->GetModuleShareDirectory();
        transperinealRobotNode->Init(vtkSlicerApplication::SafeDownCast(this->GetApplication()),moduleShareDir.c_str());
        // :TODO: this may not be the best place for robot initialization
        // (e.g., when scene is loaded from MRML the GUI will not be used)    
        transperinealRobotNode->SetAndObserveRobotConnectorNodeID(node->GetID());
        }
      else if (transperinealSmartTemplateNode = 
               vtkMRMLTransPerinealProstateSmartTemplateNode::SafeDownCast(robotNode))
        {
        transperinealSmartTemplateNode->SetAndObserveRobotConnectorNodeID(node->GetID());
        }
      }
    }
  
  if (this->ScannerConnectorSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && (event == vtkSlicerNodeSelectorWidget::NewNodeEvent || event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent))
    {
    vtkMRMLIGTLConnectorNode* node = vtkMRMLIGTLConnectorNode::SafeDownCast(this->ScannerConnectorSelector->GetSelected());
    if (robotNode && node)
      {
      vtkMRMLTransPerinealProstateRobotNode* transperinealRobotNode;
      vtkMRMLTransPerinealProstateSmartTemplateNode* transperinealSmartTemplateNode;
      if (transperinealRobotNode = 
          vtkMRMLTransPerinealProstateRobotNode::SafeDownCast(robotNode))
        {      
        transperinealRobotNode->SetAndObserveScannerConnectorNodeID(node->GetID());
        }
      else if (transperinealSmartTemplateNode = 
               vtkMRMLTransPerinealProstateSmartTemplateNode::SafeDownCast(robotNode))
        {
        transperinealSmartTemplateNode->SetAndObserveScannerConnectorNodeID(node->GetID());
        }

      }
    }
  if (this->LogFileButton->GetWidget()->GetLoadSaveDialog() == vtkKWFileBrowserDialog::SafeDownCast(caller)
      && (event == vtkKWFileBrowserDialog::FileNameChangedEvent))
    {
    const char* filename = this->LogFileButton->GetWidget()->GetLoadSaveDialog()->GetNthFileName(0);
    std::cerr << "Log file: " << filename << std::endl;
    if (robotNode)
      {
      robotNode->OpenLogFile(filename);
      }
    }
}

vtkMRMLTransPerinealProstateRobotNode* vtkProstateNavStepSetUp::GetRobotNode()
{
  if (this->ProstateNavManager==NULL)
  {
    return NULL;
  }
  vtkMRMLTransPerinealProstateRobotNode* robotNode = vtkMRMLTransPerinealProstateRobotNode::SafeDownCast(this->ProstateNavManager->GetRobotNode());
  return robotNode;
}

