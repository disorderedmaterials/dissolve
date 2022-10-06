#include "procedure/nodes/generator.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/addpair.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/calculateangle.h"
#include "procedure/nodes/calculateaxisangle.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/calculatevector.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/coordinatesets.h"
#include "procedure/nodes/cylindricalregion.h"
#include "procedure/nodes/dynamicsite.h"
#include "procedure/nodes/fit1d.h"
#include "procedure/nodes/generalregion.h"
#include "procedure/nodes/integrate1d.h"
#include "procedure/nodes/operatedivide.h"
#include "procedure/nodes/operateexpression.h"
#include "procedure/nodes/operategridnormalise.h"
#include "procedure/nodes/operatemultiply.h"
#include "procedure/nodes/operatenormalise.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/parameters.h"
#include "procedure/nodes/pick.h"
#include "procedure/nodes/pickproximity.h"
#include "procedure/nodes/pickregion.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process2d.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/remove.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1d.h"
#include "procedure/nodes/transmute.h"

NodeRef nodeGenerator(ProcedureNode::NodeType type, const SerialisedValue &node, const CoreData &data,
                      std::shared_ptr<SelectProcedureNode> parent)
{
    NodeRef result;
    switch (type)
    {
        case ProcedureNode::NodeType::Add:
            result = std::make_shared<AddProcedureNode>();
            break;
        case ProcedureNode::NodeType::AddPair:
            result = std::make_shared<AddPairProcedureNode>();
            break;
        case ProcedureNode::NodeType::Box:
            result = std::make_shared<BoxProcedureNode>();
            break;
        case ProcedureNode::NodeType::CalculateAngle:
            result = std::make_shared<CalculateAngleProcedureNode>();
            break;
        case ProcedureNode::NodeType::CalculateAxisAngle:
            result = std::make_shared<CalculateAxisAngleProcedureNode>();
            break;
        case ProcedureNode::NodeType::CalculateDistance:
            result = std::make_shared<CalculateDistanceProcedureNode>();
            break;
        case ProcedureNode::NodeType::CalculateVector:
            result = std::make_shared<CalculateVectorProcedureNode>();
            break;
        case ProcedureNode::NodeType::Collect1D:
            result = std::make_shared<Collect1DProcedureNode>();
            break;
        case ProcedureNode::NodeType::Collect2D:
            result = std::make_shared<Collect2DProcedureNode>();
            break;
        case ProcedureNode::NodeType::Collect3D:
            result = std::make_shared<Collect3DProcedureNode>();
            break;
        case ProcedureNode::NodeType::CoordinateSets:
            result = std::make_shared<CoordinateSetsProcedureNode>();
            break;
        case ProcedureNode::NodeType::CylindricalRegion:
            result = std::make_shared<CylindricalRegionProcedureNode>();
            break;
        case ProcedureNode::NodeType::DynamicSite:
            result = std::make_shared<DynamicSiteProcedureNode>(parent);
            break;
        case ProcedureNode::NodeType::Fit1D:
            result = std::make_shared<Fit1DProcedureNode>();
            break;
        case ProcedureNode::NodeType::GeneralRegion:
            result = std::make_shared<GeneralRegionProcedureNode>();
            break;
        case ProcedureNode::NodeType::Integrate1D:
            result = std::make_shared<Integrate1DProcedureNode>();
            break;
        case ProcedureNode::NodeType::OperateDivide:
            result = std::make_shared<OperateDivideProcedureNode>();
            break;
        case ProcedureNode::NodeType::OperateExpression:
            result = std::make_shared<OperateExpressionProcedureNode>();
            break;
        case ProcedureNode::NodeType::OperateGridNormalise:
            result = std::make_shared<OperateGridNormaliseProcedureNode>();
            break;
        case ProcedureNode::NodeType::OperateMultiply:
            result = std::make_shared<OperateMultiplyProcedureNode>();
            break;
        case ProcedureNode::NodeType::OperateNormalise:
            result = std::make_shared<OperateNormaliseProcedureNode>();
            break;
        case ProcedureNode::NodeType::OperateNumberDensityNormalise:
            result = std::make_shared<OperateNumberDensityNormaliseProcedureNode>();
            break;
        case ProcedureNode::NodeType::OperateSitePopulationNormalise:
            result = std::make_shared<OperateSitePopulationNormaliseProcedureNode>();
            break;
        case ProcedureNode::NodeType::OperateSphericalShellNormalise:
            result = std::make_shared<OperateSphericalShellNormaliseProcedureNode>();
            break;
        case ProcedureNode::NodeType::Parameters:
            result = std::make_shared<ParametersProcedureNode>();
            break;
        case ProcedureNode::NodeType::Pick:
            result = std::make_shared<PickProcedureNode>();
            break;
        case ProcedureNode::NodeType::PickProximity:
            result = std::make_shared<PickProximityProcedureNode>();
            break;
        case ProcedureNode::NodeType::PickRegion:
            result = std::make_shared<PickRegionProcedureNode>();
            break;
        case ProcedureNode::NodeType::Process1D:
            result = std::make_shared<Process1DProcedureNode>();
            break;
        case ProcedureNode::NodeType::Process2D:
            result = std::make_shared<Process2DProcedureNode>();
            break;
        case ProcedureNode::NodeType::Process3D:
            result = std::make_shared<Process3DProcedureNode>();
            break;
        case ProcedureNode::NodeType::Remove:
            result = std::make_shared<RemoveProcedureNode>();
            break;
        case ProcedureNode::NodeType::Select:
            result = std::make_shared<SelectProcedureNode>();
            break;
        // case ProcedureNode::NodeType::Sequence:
        //     result = std::make_shared<SequenceProcedureNode>();
        //     break;
        case ProcedureNode::NodeType::Sum1D:
            result = std::make_shared<Sum1DProcedureNode>();
            break;
        case ProcedureNode::NodeType::Transmute:
            result = std::make_shared<TransmuteProcedureNode>();
            break;
    }
    result->deserialise(node, data);
    return result;
}
