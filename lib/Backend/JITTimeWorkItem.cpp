//-------------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------

#include "Backend.h"


JITTimeWorkItem::JITTimeWorkItem(CodeGenWorkItemJITData * workItemData) :
    m_workItemData(workItemData), m_jitBody(workItemData->jitData->bodyData)
{
}

CodeGenWorkItemType
JITTimeWorkItem::Type() const
{
    return static_cast<CodeGenWorkItemType>(m_workItemData->type);
}

ExecutionMode
JITTimeWorkItem::GetJitMode() const
{
    return static_cast<ExecutionMode>(m_workItemData->jitMode);
}

// loop number if IsLoopBody, otherwise Js::LoopHeader::NoLoop
uint
JITTimeWorkItem::GetLoopNumber() const
{
    Assert(IsLoopBody() || m_workItemData->loopNumber == Js::LoopHeader::NoLoop);
    return m_workItemData->loopNumber;
}

bool
JITTimeWorkItem::IsLoopBody() const
{
    return Type() == JsLoopBodyWorkItemType;
}

bool
JITTimeWorkItem::IsJitInDebugMode() const
{
    // TODO (michhol): flags?
    return Js::Configuration::Global.EnableJitInDebugMode()
        && m_workItemData->isJitInDebugMode;
}

intptr_t
JITTimeWorkItem::GetCallsCountAddress() const
{
    Assert(Type() == JsFunctionType);

    return m_workItemData->jitData->callsCountAddress;
}

const JITLoopHeader *
JITTimeWorkItem::GetLoopHeader() const
{
    return m_jitBody.GetLoopHeaderData(GetLoopNumber());
}

intptr_t
JITTimeWorkItem::GetLoopHeaderAddr() const
{
    return m_jitBody.GetLoopHeaderAddr(GetLoopNumber());
}

void
JITTimeWorkItem::InitializeReader(
    Js::ByteCodeReader * reader,
    Js::StatementReader * statementReader)
{
    uint startOffset = IsLoopBody() ? GetLoopHeader()->startOffset : 0;
#if DBG
    reader->Create(m_jitBody.GetByteCodeBuffer(), startOffset, m_jitBody.GetByteCodeLength());
#else
    reader->Create(m_jitBody.GetByteCodeBuffer(), startOffset);
#endif
    m_jitBody.InitializeStatementMap(&m_statementMap);
    statementReader->Create(m_jitBody.GetByteCodeBuffer(), startOffset, &m_statementMap);
}

JITTimeFunctionBody *
JITTimeWorkItem::GetJITFunctionBody()
{
    return &m_jitBody;
}

CodeGenWorkItemJITData *
JITTimeWorkItem::GetWorkItemData()
{
    return m_workItemData;
}

void
JITTimeWorkItem::SetJITTimeData(FunctionJITTimeData * jitData)
{
    m_workItemData->jitData = jitData;
}

const FunctionJITTimeInfo *
JITTimeWorkItem::GetJITTimeInfo() const
{
    return reinterpret_cast<const FunctionJITTimeInfo *>(m_workItemData->jitData);
}
