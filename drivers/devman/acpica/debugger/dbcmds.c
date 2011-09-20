/*******************************************************************************
 *
 * Module Name: dbcmds - Miscellaneous debug commands and output routines
 *
 ******************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2011, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights.  You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code.  No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision.  In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change.  Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee.  Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution.  In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE.  ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT,  ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES.  INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS.  INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government.  In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/


#include "acpi.h"
#include "accommon.h"
#include "acevents.h"
#include "acdebug.h"
#include "acresrc.h"
#include "actables.h"

#ifdef ACPI_DEBUGGER

#define _COMPONENT          ACPI_CA_DEBUGGER
        ACPI_MODULE_NAME    ("dbcmds")


/* Local prototypes */

static void
AcpiDmCompareAmlResources (
    UINT8                   *Aml1Buffer,
    ACPI_RSDESC_SIZE        Aml1BufferLength,
    UINT8                   *Aml2Buffer,
    ACPI_RSDESC_SIZE        Aml2BufferLength);

static ACPI_STATUS
AcpiDmTestResourceConversion (
    ACPI_NAMESPACE_NODE     *Node,
    char                    *Name);


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbConvertToNode
 *
 * PARAMETERS:  InString        - String to convert
 *
 * RETURN:      Pointer to a NS node
 *
 * DESCRIPTION: Convert a string to a valid NS pointer.  Handles numeric or
 *              alpha strings.
 *
 ******************************************************************************/

ACPI_NAMESPACE_NODE *
AcpiDbConvertToNode (
    char                    *InString)
{
    ACPI_NAMESPACE_NODE     *Node;


    if ((*InString >= 0x30) && (*InString <= 0x39))
    {
        /* Numeric argument, convert */

        Node = ACPI_TO_POINTER (ACPI_STRTOUL (InString, NULL, 16));
        if (!AcpiOsReadable (Node, sizeof (ACPI_NAMESPACE_NODE)))
        {
            AcpiOsPrintf ("Address %p is invalid in this address space\n",
                Node);
            return (NULL);
        }

        /* Make sure pointer is valid NS node */

        if (ACPI_GET_DESCRIPTOR_TYPE (Node) != ACPI_DESC_TYPE_NAMED)
        {
            AcpiOsPrintf ("Address %p is not a valid NS node [%s]\n",
                    Node, AcpiUtGetDescriptorName (Node));
            return (NULL);
        }
    }
    else
    {
        /* Alpha argument */
        /* The parameter is a name string that must be resolved to a
         * Named obj
         */
        Node = AcpiDbLocalNsLookup (InString);
        if (!Node)
        {
            Node = AcpiGbl_RootNode;
        }
    }

    return (Node);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbSleep
 *
 * PARAMETERS:  ObjectArg       - Desired sleep state (0-5)
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Simulate a sleep/wake sequence
 *
 ******************************************************************************/

ACPI_STATUS
AcpiDbSleep (
    char                    *ObjectArg)
{
    ACPI_STATUS             Status;
    UINT8                   SleepState;


    SleepState = (UINT8) ACPI_STRTOUL (ObjectArg, NULL, 0);

    AcpiOsPrintf ("**** Prepare to sleep ****\n");
    Status = AcpiEnterSleepStatePrep (SleepState);
    if (ACPI_FAILURE (Status))
    {
        return (Status);
    }

    AcpiOsPrintf ("**** Going to sleep ****\n");
    Status = AcpiEnterSleepState (SleepState);
    if (ACPI_FAILURE (Status))
    {
        return (Status);
    }

    AcpiOsPrintf ("**** returning from sleep ****\n");
    Status = AcpiLeaveSleepState (SleepState);

    return (Status);
}

/*******************************************************************************
 *
 * FUNCTION:    AcpiDbDisplayLocks
 *
 * PARAMETERS:  None
 *
 * RETURN:      None
 *
 * DESCRIPTION: Display information about internal mutexes.
 *
 ******************************************************************************/

void
AcpiDbDisplayLocks (
    void)
{
    UINT32                  i;


    for (i = 0; i < ACPI_MAX_MUTEX; i++)
    {
        AcpiOsPrintf ("%26s : %s\n", AcpiUtGetMutexName (i),
            AcpiGbl_MutexInfo[i].ThreadId == ACPI_MUTEX_NOT_ACQUIRED
                ? "Locked" : "Unlocked");
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbDisplayTableInfo
 *
 * PARAMETERS:  TableArg        - String with name of table to be displayed
 *
 * RETURN:      None
 *
 * DESCRIPTION: Display information about loaded tables.  Current
 *              implementation displays all loaded tables.
 *
 ******************************************************************************/

void
AcpiDbDisplayTableInfo (
    char                    *TableArg)
{
    UINT32                  i;
    ACPI_TABLE_DESC         *TableDesc;
    ACPI_STATUS             Status;


    /* Walk the entire root table list */

    for (i = 0; i < AcpiGbl_RootTableList.CurrentTableCount; i++)
    {
        TableDesc = &AcpiGbl_RootTableList.Tables[i];
        AcpiOsPrintf ("%u ", i);

        /* Make sure that the table is mapped */

        Status = AcpiTbVerifyTable (TableDesc);
        if (ACPI_FAILURE (Status))
        {
            return;
        }

        /* Dump the table header */

        if (TableDesc->Pointer)
        {
            AcpiTbPrintTableHeader (TableDesc->Address, TableDesc->Pointer);
        }
        else
        {
            /* If the pointer is null, the table has been unloaded */

            ACPI_INFO ((AE_INFO, "%4.4s - Table has been unloaded",
                TableDesc->Signature.Ascii));
        }
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbUnloadAcpiTable
 *
 * PARAMETERS:  TableArg        - Name of the table to be unloaded
 *              InstanceArg     - Which instance of the table to unload (if
 *                                there are multiple tables of the same type)
 *
 * RETURN:      Nonde
 *
 * DESCRIPTION: Unload an ACPI table.
 *              Instance is not implemented
 *
 ******************************************************************************/

void
AcpiDbUnloadAcpiTable (
    char                    *TableArg,
    char                    *InstanceArg)
{
/* TBD: Need to reimplement for new data structures */

#if 0
    UINT32                  i;
    ACPI_STATUS             Status;


    /* Search all tables for the target type */

    for (i = 0; i < (ACPI_TABLE_ID_MAX+1); i++)
    {
        if (!ACPI_STRNCMP (TableArg, AcpiGbl_TableData[i].Signature,
                AcpiGbl_TableData[i].SigLength))
        {
            /* Found the table, unload it */

            Status = AcpiUnloadTable (i);
            if (ACPI_SUCCESS (Status))
            {
                AcpiOsPrintf ("[%s] unloaded and uninstalled\n", TableArg);
            }
            else
            {
                AcpiOsPrintf ("%s, while unloading [%s]\n",
                    AcpiFormatException (Status), TableArg);
            }

            return;
        }
    }

    AcpiOsPrintf ("Unknown table type [%s]\n", TableArg);
#endif
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbSendNotify
 *
 * PARAMETERS:  Name            - Name of ACPI object to send the notify to
 *              Value           - Value of the notify to send.
 *
 * RETURN:      None
 *
 * DESCRIPTION: Send an ACPI notification.  The value specified is sent to the
 *              named object as an ACPI notify.
 *
 ******************************************************************************/

void
AcpiDbSendNotify (
    char                    *Name,
    UINT32                  Value)
{
    ACPI_NAMESPACE_NODE     *Node;
    ACPI_STATUS             Status;


    /* Translate name to an Named object */

    Node = AcpiDbConvertToNode (Name);
    if (!Node)
    {
        return;
    }

    /* Decode Named object type */

    switch (Node->Type)
    {
    case ACPI_TYPE_DEVICE:
    case ACPI_TYPE_THERMAL:

         /* Send the notify */

        Status = AcpiEvQueueNotifyRequest (Node, Value);
        if (ACPI_FAILURE (Status))
        {
            AcpiOsPrintf ("Could not queue notify\n");
        }
        break;

    default:
        AcpiOsPrintf ("Named object is not a device or a thermal object\n");
        break;
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbDisplayInterfaces
 *
 * PARAMETERS:  ActionArg           - Null, "install", or "remove"
 *              InterfaceNameArg    - Name for install/remove options
 *
 * RETURN:      None
 *
 * DESCRIPTION: Display or modify the global _OSI interface list
 *
 ******************************************************************************/

void
AcpiDbDisplayInterfaces (
    char                    *ActionArg,
    char                    *InterfaceNameArg)
{
    ACPI_INTERFACE_INFO     *NextInterface;
    char                    *SubString;
    ACPI_STATUS             Status;


    /* If no arguments, just display current interface list */

    if (!ActionArg)
    {
        (void) AcpiOsAcquireMutex (AcpiGbl_OsiMutex,
                    ACPI_WAIT_FOREVER);

        NextInterface = AcpiGbl_SupportedInterfaces;

        while (NextInterface)
        {
            if (!(NextInterface->Flags & ACPI_OSI_INVALID))
            {
                AcpiOsPrintf ("%s\n", NextInterface->Name);
            }
            NextInterface = NextInterface->Next;
        }

        AcpiOsReleaseMutex (AcpiGbl_OsiMutex);
        return;
    }

    /* If ActionArg exists, so must InterfaceNameArg */

    if (!InterfaceNameArg)
    {
        AcpiOsPrintf ("Missing Interface Name argument\n");
        return;
    }

    /* Uppercase the action for match below */

    AcpiUtStrupr (ActionArg);

    /* Install - install an interface */

    SubString = ACPI_STRSTR ("INSTALL", ActionArg);
    if (SubString)
    {
        Status = AcpiInstallInterface (InterfaceNameArg);
        if (ACPI_FAILURE (Status))
        {
            AcpiOsPrintf ("%s, while installing \"%s\"\n",
                AcpiFormatException (Status), InterfaceNameArg);
        }
        return;
    }

    /* Remove - remove an interface */

    SubString = ACPI_STRSTR ("REMOVE", ActionArg);
    if (SubString)
    {
        Status = AcpiRemoveInterface (InterfaceNameArg);
        if (ACPI_FAILURE (Status))
        {
            AcpiOsPrintf ("%s, while removing \"%s\"\n",
                AcpiFormatException (Status), InterfaceNameArg);
        }
        return;
    }

    /* Invalid ActionArg */

    AcpiOsPrintf ("Invalid action argument: %s\n", ActionArg);
    return;
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDmCompareAmlResources
 *
 * PARAMETERS:  Aml1Buffer          - Contains first resource list
 *              Aml1BufferLength    - Length of first resource list
 *              Aml2Buffer          - Contains second resource list
 *              Aml2BufferLength    - Length of second resource list
 *
 * RETURN:      None
 *
 * DESCRIPTION: Compare two AML resource lists, descriptor by descriptor (in
 *              order to isolate a miscompare to an individual resource)
 *
 ******************************************************************************/

static void
AcpiDmCompareAmlResources (
    UINT8                   *Aml1Buffer,
    ACPI_RSDESC_SIZE        Aml1BufferLength,
    UINT8                   *Aml2Buffer,
    ACPI_RSDESC_SIZE        Aml2BufferLength)
{
    UINT8                   *Aml1;
    UINT8                   *Aml2;
    ACPI_RSDESC_SIZE        Aml1Length;
    ACPI_RSDESC_SIZE        Aml2Length;
    ACPI_RSDESC_SIZE        Offset = 0;
    UINT8                   ResourceType;
    UINT32                  Count = 0;


    /* Compare overall buffer sizes (may be different due to size rounding) */

    if (Aml1BufferLength != Aml2BufferLength)
    {
        AcpiOsPrintf (
            "**** Buffer length mismatch in converted AML: original %X new %X ****\n",
            Aml1BufferLength, Aml2BufferLength);
    }

    Aml1 = Aml1Buffer;
    Aml2 = Aml2Buffer;

    /* Walk the descriptor lists, comparing each descriptor */

    while (Aml1 < (Aml1Buffer + Aml1BufferLength))
    {
        /* Get the lengths of each descriptor */

        Aml1Length = AcpiUtGetDescriptorLength (Aml1);
        Aml2Length = AcpiUtGetDescriptorLength (Aml2);
        ResourceType = AcpiUtGetResourceType (Aml1);

        /* Check for descriptor length match */

        if (Aml1Length != Aml2Length)
        {
            AcpiOsPrintf (
                "**** Length mismatch in descriptor [%.2X] type %2.2X, Offset %8.8X L1 %X L2 %X ****\n",
                Count, ResourceType, Offset, Aml1Length, Aml2Length);
        }

        /* Check for descriptor byte match */

        else if (ACPI_MEMCMP (Aml1, Aml2, Aml1Length))
        {
            AcpiOsPrintf (
                "**** Data mismatch in descriptor [%.2X] type %2.2X, Offset %8.8X ****\n",
                Count, ResourceType, Offset);
        }

        /* Exit on EndTag descriptor */

        if (ResourceType == ACPI_RESOURCE_NAME_END_TAG)
        {
            return;
        }

        /* Point to next descriptor in each buffer */

        Count++;
        Offset += Aml1Length;
        Aml1 += Aml1Length;
        Aml2 += Aml2Length;
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDmTestResourceConversion
 *
 * PARAMETERS:  Node            - Parent device node
 *              Name            - resource method name (_CRS)
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Compare the original AML with a conversion of the AML to
 *              internal resource list, then back to AML.
 *
 ******************************************************************************/

static ACPI_STATUS
AcpiDmTestResourceConversion (
    ACPI_NAMESPACE_NODE     *Node,
    char                    *Name)
{
    ACPI_STATUS             Status;
    ACPI_BUFFER             ReturnObj;
    ACPI_BUFFER             ResourceObj;
    ACPI_BUFFER             NewAml;
    ACPI_OBJECT             *OriginalAml;


    AcpiOsPrintf ("Resource Conversion Comparison:\n");

    NewAml.Length = ACPI_ALLOCATE_LOCAL_BUFFER;
    ReturnObj.Length = ACPI_ALLOCATE_LOCAL_BUFFER;
    ResourceObj.Length = ACPI_ALLOCATE_LOCAL_BUFFER;

    /* Get the original _CRS AML resource template */

    Status = AcpiEvaluateObject (Node, Name, NULL, &ReturnObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("Could not obtain %s: %s\n",
            Name, AcpiFormatException (Status));
        return (Status);
    }

    /* Get the AML resource template, converted to internal resource structs */

    Status = AcpiGetCurrentResources (Node, &ResourceObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("AcpiGetCurrentResources failed: %s\n",
            AcpiFormatException (Status));
        goto Exit1;
    }

    /* Convert internal resource list to external AML resource template */

    Status = AcpiRsCreateAmlResources (ResourceObj.Pointer, &NewAml);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("AcpiRsCreateAmlResources failed: %s\n",
            AcpiFormatException (Status));
        goto Exit2;
    }

    /* Compare original AML to the newly created AML resource list */

    OriginalAml = ReturnObj.Pointer;

    AcpiDmCompareAmlResources (
        OriginalAml->Buffer.Pointer, (ACPI_RSDESC_SIZE) OriginalAml->Buffer.Length,
        NewAml.Pointer, (ACPI_RSDESC_SIZE) NewAml.Length);

    /* Cleanup and exit */

    ACPI_FREE (NewAml.Pointer);
Exit2:
    ACPI_FREE (ResourceObj.Pointer);
Exit1:
    ACPI_FREE (ReturnObj.Pointer);
    return (Status);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbDisplayResources
 *
 * PARAMETERS:  ObjectArg       - String with hex value of the object
 *
 * RETURN:      None
 *
 * DESCRIPTION: Display the resource objects associated with a device.
 *
 ******************************************************************************/

void
AcpiDbDisplayResources (
    char                    *ObjectArg)
{
    ACPI_NAMESPACE_NODE     *Node;
    ACPI_STATUS             Status;
    ACPI_BUFFER             ReturnObj;


    AcpiDbSetOutputDestination (ACPI_DB_REDIRECTABLE_OUTPUT);
    AcpiDbgLevel |= ACPI_LV_RESOURCES;

    /* Convert string to object pointer */

    Node = AcpiDbConvertToNode (ObjectArg);
    if (!Node)
    {
        return;
    }

    /* Prepare for a return object of arbitrary size */

    ReturnObj.Pointer = AcpiGbl_DbBuffer;
    ReturnObj.Length  = ACPI_DEBUG_BUFFER_SIZE;

    /* _PRT */

    AcpiOsPrintf ("Evaluating _PRT\n");

    /* Check if _PRT exists */

    Status = AcpiEvaluateObject (Node, METHOD_NAME__PRT, NULL, &ReturnObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("Could not obtain _PRT: %s\n",
            AcpiFormatException (Status));
        goto GetCrs;
    }

    ReturnObj.Pointer = AcpiGbl_DbBuffer;
    ReturnObj.Length  = ACPI_DEBUG_BUFFER_SIZE;

    Status = AcpiGetIrqRoutingTable (Node, &ReturnObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("GetIrqRoutingTable failed: %s\n",
            AcpiFormatException (Status));
        goto GetCrs;
    }

    AcpiRsDumpIrqList (ACPI_CAST_PTR (UINT8, AcpiGbl_DbBuffer));


    /* _CRS */

GetCrs:
    AcpiOsPrintf ("Evaluating _CRS\n");

    ReturnObj.Pointer = AcpiGbl_DbBuffer;
    ReturnObj.Length  = ACPI_DEBUG_BUFFER_SIZE;

    /* Check if _CRS exists */

    Status = AcpiEvaluateObject (Node, METHOD_NAME__CRS, NULL, &ReturnObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("Could not obtain _CRS: %s\n",
            AcpiFormatException (Status));
        goto GetPrs;
    }

    /* Get the _CRS resource list */

    ReturnObj.Pointer = AcpiGbl_DbBuffer;
    ReturnObj.Length  = ACPI_DEBUG_BUFFER_SIZE;

    Status = AcpiGetCurrentResources (Node, &ReturnObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("AcpiGetCurrentResources failed: %s\n",
            AcpiFormatException (Status));
        goto GetPrs;
    }

    /* Dump the _CRS resource list */

    AcpiRsDumpResourceList (ACPI_CAST_PTR (ACPI_RESOURCE,
        ReturnObj.Pointer));

    /*
     * Perform comparison of original AML to newly created AML. This tests both
     * the AML->Resource conversion and the Resource->Aml conversion.
     */
    Status = AcpiDmTestResourceConversion (Node, METHOD_NAME__CRS);

    /* Execute _SRS with the resource list */

    Status = AcpiSetCurrentResources (Node, &ReturnObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("AcpiSetCurrentResources failed: %s\n",
            AcpiFormatException (Status));
        goto GetPrs;
    }


    /* _PRS */

GetPrs:
    AcpiOsPrintf ("Evaluating _PRS\n");

    ReturnObj.Pointer = AcpiGbl_DbBuffer;
    ReturnObj.Length  = ACPI_DEBUG_BUFFER_SIZE;

    /* Check if _PRS exists */

    Status = AcpiEvaluateObject (Node, METHOD_NAME__PRS, NULL, &ReturnObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("Could not obtain _PRS: %s\n",
            AcpiFormatException (Status));
        goto Cleanup;
    }

    ReturnObj.Pointer = AcpiGbl_DbBuffer;
    ReturnObj.Length  = ACPI_DEBUG_BUFFER_SIZE;

    Status = AcpiGetPossibleResources (Node, &ReturnObj);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("AcpiGetPossibleResources failed: %s\n",
            AcpiFormatException (Status));
        goto Cleanup;
    }

    AcpiRsDumpResourceList (ACPI_CAST_PTR (ACPI_RESOURCE, AcpiGbl_DbBuffer));

Cleanup:

    AcpiDbSetOutputDestination (ACPI_DB_CONSOLE_OUTPUT);
    return;
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbGenerateGpe
 *
 * PARAMETERS:  GpeArg          - Raw GPE number, ascii string
 *              BlockArg        - GPE block number, ascii string
 *                                0 or 1 for FADT GPE blocks
 *
 * RETURN:      None
 *
 * DESCRIPTION: Generate a GPE
 *
 ******************************************************************************/

void
AcpiDbGenerateGpe (
    char                    *GpeArg,
    char                    *BlockArg)
{
    UINT32                  BlockNumber;
    UINT32                  GpeNumber;
    ACPI_GPE_EVENT_INFO     *GpeEventInfo;


    GpeNumber   = ACPI_STRTOUL (GpeArg, NULL, 0);
    BlockNumber = ACPI_STRTOUL (BlockArg, NULL, 0);


    GpeEventInfo = AcpiEvGetGpeEventInfo (ACPI_TO_POINTER (BlockNumber),
        GpeNumber);
    if (!GpeEventInfo)
    {
        AcpiOsPrintf ("Invalid GPE\n");
        return;
    }

    (void) AcpiEvGpeDispatch (NULL, GpeEventInfo, GpeNumber);
}

#endif /* ACPI_DEBUGGER */
