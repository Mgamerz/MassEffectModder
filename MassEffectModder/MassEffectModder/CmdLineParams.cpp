/*
 * MassEffectModder
 *
 * Copyright (C) 2018-2019 Pawel Kolodziejski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "Exceptions/SignalHandler.h"
#include "Helpers/MiscHelpers.h"
#include "Helpers/Logs.h"

#include "CmdLineParams.h"
#include "CmdLineTools.h"
#include "ConfigIni.h"
#include "GameData.h"
#include "MemTypes.h"
#include "TOCFile.h"
#include "DLC.h"
#include "Misc.h"

static bool hasValue(const QStringList &args, int curPos)
{
    return args.count() >= (curPos + 2) && !args[curPos + 1].startsWith("--");
}

static QStringList convertLegacyArguments()
{
    QStringList retArgs;
    const QStringList args = QCoreApplication::arguments();
    if (args.count() < 2)
        return QStringList();

    retArgs.append("");
    if (args[1] == "-check-game-data-after")
    {
        if (args.count() > 1)
        {
            retArgs.append("--check-game-data-after");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-check-game-data-mismatch")
    {
        if (args.count() > 1)
        {
            retArgs.append("--check-game-data-mismatch");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-check-game-data-only-vanilla")
    {
        if (args.count() > 1)
        {
            retArgs.append("--check-game-data-vanilla");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-check-game-data-textures")
    {
        if (args.count() > 1)
        {
            retArgs.append("--check-game-data-textures");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-check-for-markers")
    {
        if (args.count() > 1)
        {
            retArgs.append("--check-for-markers");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-install-mods")
    {
        if (args.count() > 2)
        {
            retArgs.append("--install-mods");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--input");
            retArgs.append(args[3]);
            for (int a = 0; a < args.count(); a++)
            {
                if (args[a] == "-repack")
                    retArgs.append("--repack-mode");
            }
            retArgs.append("--ipc");
        }

    }
    else if (args[1] == "-apply-me1-laa")
    {
        retArgs.append("--apply-me1-laa");
    }
    else if (args[1] == "-detect-mods")
    {
        if (args.count() > 1)
        {
            retArgs.append("--detect-mods");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-detect-bad-mods")
    {
        if (args.count() > 1)
        {
            retArgs.append("--detect-bad-mods");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-apply-lods-gfx")
    {
        if (args.count() > 1)
        {
            retArgs.append("--apply-lods-gfx");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            for (int a = 0; a < args.count(); a++)
            {
                if (args[a] == "-soft-shadows-mode")
                    retArgs.append("--soft-shadows-mode");
            }
            for (int a = 0; a < args.count(); a++)
            {
                if (args[a] == "-meuitm-mode")
                    retArgs.append("--meuitm-mode");
            }
            for (int a = 0; a < args.count(); a++)
            {
                if (args[a] == "-limit2k")
                    retArgs.append("--limit-2k");
            }
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-remove-lods")
    {
        if (args.count() > 1)
        {
            retArgs.append("--remove-lods");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-print-lods")
    {
        if (args.count() > 1)
        {
            retArgs.append("--print-lods");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-convert-to-mem")
    {
        if (args.count() > 3)
        {
            retArgs.append("--convert-to-mem");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--input");
            retArgs.append(args[3]);
            retArgs.append("--output");
            retArgs.append(args[4]);
            for (int a = 0; a < args.count(); a++)
            {
                if (args[a] == "--mark-to-convert")
                    retArgs.append("--mark-to-convert");
            }
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-extract-mod")
    {
        if (args.count() > 3)
        {
            retArgs.append("--extract-mod");
            retArgs.append("--gameid");
            retArgs.append(args[2]);
            retArgs.append("--input");
            retArgs.append(args[3]);
            retArgs.append("--output");
            retArgs.append(args[4]);
            retArgs.append("--ipc");
        }
    }
    else if (args[1] == "-extract-tpf")
    {
        if (args.count() > 2)
        {
            retArgs.append("--extract-tpf");
            retArgs.append("--input");
            retArgs.append(args[2]);
            retArgs.append("--output");
            retArgs.append(args[3]);
            retArgs.append("--ipc");
        }
    }
    else
    {
        return args;
    }

    return retArgs;
}

int ProcessArguments()
{
    int errorCode = 0;
    int cmd = CmdType::UNKNOWN;
    MeType gameId = MeType::UNKNOWN_TYPE;
    bool markToConvert = false;
    bool guiMode = false;
    bool repackMode = false;
    bool meuitmMode = false;
    bool softShadowsMods = false;
    bool limit2k = false;
    bool pccOnly = false;
    bool tfcOnly = false;
    bool verify = false;
    int thresholdValue = 128;
    QString input, output, threshold, format, tfcName, guid, path;
    CmdLineTools tools;

    QStringList args = convertLegacyArguments();
    if (args.count() != 0)
        args.removeFirst();

    for (int l = 0; l < args.count(); l++)
    {
        const QString arg = args[l].toLower();
        if (arg == "--help")
            cmd = CmdType::HELP;
        else if (arg == "--scan")
            cmd = CmdType::SCAN;
        else if (arg == "--remove-empty-mips")
            cmd = CmdType::REMOVE_EMPTY_MIPS;
        else if (arg == "--update-toc")
            cmd = CmdType::UPDATE_TOC;
        else if (arg == "--unpack-dlcs")
            cmd = CmdType::UNPACK_DLCS;
        else if (arg == "--repack")
            cmd = CmdType::REPACK;
        else if (arg == "--convert-to-mem")
            cmd = CmdType::CONVERT_TO_MEM;
        else if (arg == "--convert-game-image")
            cmd = CmdType::CONVERT_GAME_IMAGE;
        else if (arg == "--convert-game-images")
            cmd = CmdType::CONVERT_GAME_IMAGES;
        else if (arg == "--convert-image")
            cmd = CmdType::CONVERT_IMAGE;
        else if (arg == "--install-mods")
            cmd = CmdType::INSTALL_MODS;
        else if (arg == "--extract-mod")
            cmd = CmdType::EXTRACT_MOD;
        else if (arg == "--extract-mem")
            cmd = CmdType::EXTRACT_MEM;
        else if (arg == "--extract-tpf")
            cmd = CmdType::EXTRACT_TPF;
        else if (arg == "--detect-mods")
            cmd = CmdType::DETECT_MODS;
        else if (arg == "--detect-bad-mods")
            cmd = CmdType::DETECT_BAD_MODS;
        else if (arg == "--apply-lods-gfx")
            cmd = CmdType::APPLY_LODS_GFX;
        else if (arg == "--print-lods")
            cmd = CmdType::PRINT_LODS;
        else if (arg == "--remove-lods")
            cmd = CmdType::REMOVE_LODS;
        else if (arg == "--apply-me1-laa")
            cmd = CmdType::APPLY_ME1_LAA;
        else if (arg == "--check-game-data-textures")
            cmd = CmdType::CHECK_GAME_DATA_TEXTURES;
        else if (arg == "--check-game-data-mismatch")
            cmd = CmdType::CHECK_GAME_DATA_MISMATCH;
        else if (arg == "--check-game-data-after")
            cmd = CmdType::CHECK_GAME_DATA_AFTER;
        else if (arg == "--check-game-data-vanilla")
            cmd = CmdType::CHECK_GAME_DATA_VANILLA;
        else if (arg == "--check-for-markers")
            cmd = CmdType::CHECK_FOR_MARKERS;
        else if (arg == "--extract-all-dds")
            cmd = CmdType::EXTRACT_ALL_DDS;
        else if (arg == "--extract-all-png")
            cmd = CmdType::EXTRACT_ALL_PNG;
        else if (arg == "--dlc-mod-textures")
            cmd = CmdType::DLC_MOD_TEXTURES;
        else if (arg == "--unpack-archive")
            cmd = CmdType::UNPACK_ARCHIVE;
#if !defined(_WIN32)
        else if (arg == "--set-game-data-path")
            cmd = CmdType::SET_GAME_DATA_PATH;
        else if (arg == "--set-game-user-path")
            cmd = CmdType::SET_GAME_USER_PATH;
#endif
        else
            continue;
        args.removeAt(l);
        break;
    }

    switch (cmd)
    {
    case CmdType::HELP:
        DisplayHelp();
        return 0;
    case CmdType::UNKNOWN:
        PERROR("Wrong command!\n");
        DisplayHelp();
        return 1;
    }

    for (int l = 0; l < args.count(); l++)
    {
        const QString arg = args[l].toLower();
        if (arg == "--gameid" && hasValue(args, l))
        {
            bool ok;
            int id = args[l + 1].toInt(&ok);
            if (ok && id >= 1 && id <= 3)
            {
                gameId = (MeType)id;
            }
            args.removeAt(l);
            args.removeAt(l--);
        }
        else if (arg == "--ipc")
        {
            g_ipc = true;
            args.removeAt(l--);
        }
        else if (arg == "--input" && hasValue(args, l))
        {
            input = args[l + 1].replace('\\', '/');
            if (input.length() == 0)
            {
                PERROR("Input path param wrong!\n");
                return -1;
            }
            args.removeAt(l);
            args.removeAt(l--);
        }
        else if (arg == "--output" && hasValue(args, l))
        {
            output = args[l + 1].replace('\\', '/');
            if (output.length() == 0)
            {
                PERROR("Output path wrong!\n");
                return -1;
            }
            args.removeAt(l);
            args.removeAt(l--);
        }
        else if (arg == "--mark-to-convert")
        {
            markToConvert = true;
            args.removeAt(l--);
        }
        else if (arg == "--alot-mode")
        {
            guiMode = true;
            args.removeAt(l--);
        }
        else if (arg == "--repack-mode")
        {
            repackMode = true;
            args.removeAt(l--);
        }
        else if (arg == "--soft-shadows-mode")
        {
            softShadowsMods = true;
            args.removeAt(l--);
        }
        else if (arg == "--meuitm-mode")
        {
            meuitmMode = true;
            args.removeAt(l--);
        }
        else if (arg == "--limit-2k")
        {
            limit2k = true;
            args.removeAt(l--);
        }
        else if (arg == "--verify")
        {
            verify = true;
            args.removeAt(l--);
        }
        else if (arg == "--pcc-only")
        {
            if (!tfcOnly)
                pccOnly = true;
            args.removeAt(l--);
        }
        else if (arg == "--tfc-only")
        {
            if (!tfcOnly)
                pccOnly = true;
            tfcOnly = true;
            args.removeAt(l--);
        }
        else if (arg == "--threshold" && hasValue(args, l))
        {
            threshold = args[l + 1];
            if (threshold.length() != 0)
            {
                thresholdValue = threshold.toInt();
            }
            args.removeAt(l);
            args.removeAt(l--);
        }
        else if (arg == "--format" && hasValue(args, l))
        {
            format = args[l + 1];
            args.removeAt(l);
            args.removeAt(l--);
        }
        else if (arg == "--tfc-name" && hasValue(args, l))
        {
            if (!pccOnly && !tfcOnly)
                tfcName = args[l + 1];
            args.removeAt(l);
            args.removeAt(l--);
        }
        else if (arg == "--guid" && hasValue(args, l))
        {
            guid = args[l + 1];
            args.removeAt(l);
            args.removeAt(l--);
        }
        else if (arg == "--debug-logs")
        {
            g_logs->ChangeLogLevel(LOG_DEBUG);
            args.removeAt(l--);
        }
        else if (arg == "--path" && hasValue(args, l))
        {
            path = args[l + 1];
            args.removeAt(l);
            args.removeAt(l--);
        }
    }
    if (args.count() != 0)
    {
        PERROR("Wrong options!\n");
        DisplayHelp();
        return 1;
    }

    switch (cmd)
    {
    case CmdType::SCAN:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        errorCode = tools.scanTextures(gameId);
        break;
    case CmdType::REMOVE_EMPTY_MIPS:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        errorCode = tools.removeEmptyMips(gameId);
        break;
    case CmdType::UPDATE_TOC:
    {
        if (!tools.updateTOCs())
            errorCode = 1;
        break;
    }
    case CmdType::UNPACK_DLCS:
    {
        if (!tools.unpackAllDLCs())
            errorCode = 1;
        break;
    }
    case CmdType::REPACK:
    {
        if (gameId != MeType::ME2_TYPE && gameId != MeType::ME3_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.repackGame(gameId))
            errorCode = 1;
        break;
    }
    case CmdType::CONVERT_TO_MEM:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!QDir(input).exists())
        {
            PERROR("Input folder doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        if (output.length() == 0)
        {
            PERROR("Output param empty!\n");
            errorCode = 1;
            break;
        }
        if (!output.endsWith(".mem", Qt::CaseInsensitive))
        {
            PERROR(QString("Error: output file is not mem: ") + output + "\n");
            errorCode = 1;
            break;
        }
        if (!tools.ConvertToMEM(gameId, input, output, markToConvert))
            errorCode = 1;
        break;
    case CmdType::CONVERT_GAME_IMAGE:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!QFile(input).exists())
        {
            PERROR("Input file doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        if (!output.endsWith(".dds", Qt::CaseInsensitive))
        {
            PERROR(QString("Error: output file is not dds: ") + output + "\n");
            errorCode = 1;
            break;
        }
        if (!tools.convertGameImage(gameId, input, output, markToConvert))
            errorCode = 1;
        break;
    case CmdType::CONVERT_IMAGE:
        if (!QFile(input).exists())
        {
            PERROR("Input file doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        if (output.length() == 0)
        {
            PERROR("Output param empty!\n");
            errorCode = 1;
            break;
        }
        if (!tools.convertImage(input, output, format, thresholdValue))
            errorCode = 1;
        break;
    case CmdType::CONVERT_GAME_IMAGES:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!QDir(input).exists())
        {
            PERROR("Input folder doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        if (output.length() == 0)
        {
            PERROR("Output param empty!\n");
            errorCode = 1;
            break;
        }
        if (!tools.convertGameImages(gameId, input, output, markToConvert))
            errorCode = 1;
        break;
    case CmdType::INSTALL_MODS:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!QDir(input).exists())
        {
            PERROR("Input folder doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        if (!tools.InstallMods(gameId, input, repackMode, guiMode, limit2k, verify))
        {
            errorCode = 1;
        }
        break;
    case CmdType::EXTRACT_MOD:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (input.endsWith(".mod", Qt::CaseInsensitive))
        {
            if (!QFile(input).exists())
            {
                PERROR("Input file doesn't exists! " + input + "\n");
                errorCode = 1;
                break;
            }
        }
        else if (!QDir(input).exists())
        {
            PERROR("Input folder doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        if (!tools.extractMOD(gameId, input, output))
            errorCode = 1;
        break;
    case CmdType::EXTRACT_MEM:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (input.endsWith(".mem", Qt::CaseInsensitive))
        {
            if (!QFile(input).exists())
            {
                PERROR("Input file doesn't exists! " + input + "\n");
                errorCode = 1;
                break;
            }
        }
        else if (!QDir(input).exists())
        {
            PERROR("Input folder doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        if (!tools.extractMEM(gameId, input, output))
            errorCode = 1;
        break;
    case CmdType::EXTRACT_TPF:
        if (input.endsWith(".tpf", Qt::CaseInsensitive))
        {
            if (!QFile(input).exists())
            {
                PERROR("Input file doesn't exists! " + input + "\n");
                errorCode = 1;
                break;
            }
        }
        else if (!QDir(input).exists())
        {
            PERROR("Input folder doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        if (!tools.extractTPF(input, output))
            errorCode = 1;
        break;
    case CmdType::DETECT_MODS:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.DetectMods(gameId))
            errorCode = 1;
        break;
    case CmdType::DETECT_BAD_MODS:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.DetectBadMods(gameId))
            errorCode = 1;
        break;
    case CmdType::APPLY_LODS_GFX:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.ApplyLODAndGfxSettings(gameId, softShadowsMods, meuitmMode, limit2k))
            errorCode = 1;
        break;
    case CmdType::PRINT_LODS:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.PrintLODSettings(gameId))
            errorCode = 1;
        break;
    case CmdType::REMOVE_LODS:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.RemoveLODSettings(gameId))
            errorCode = 1;
        break;
    case CmdType::APPLY_ME1_LAA:
        if (!tools.ApplyME1LAAPatch())
            errorCode = 1;
        break;
    case CmdType::CHECK_GAME_DATA_TEXTURES:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.CheckTextures(gameId))
            errorCode = 1;
        break;
    case CmdType::CHECK_GAME_DATA_AFTER:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.checkGameFilesAfter(gameId))
            errorCode = 1;
        break;
    case CmdType::CHECK_GAME_DATA_MISMATCH:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.detectsMismatchPackagesAfter(gameId))
            errorCode = 1;
        break;
    case CmdType::CHECK_GAME_DATA_VANILLA:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!");
            errorCode = 1;
            break;
        }
        if (!tools.CheckGameData(gameId))
            errorCode = 1;
        break;
    case CmdType::CHECK_FOR_MARKERS:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (!tools.CheckForMarkers(gameId))
            errorCode = 1;
        break;
    case CmdType::EXTRACT_ALL_DDS:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (output.length() == 0)
        {
            PERROR("Output param empty!\n");
            errorCode = 1;
            break;
        }
        if (!tools.extractAllTextures(gameId, output, false, pccOnly, tfcOnly, tfcName))
            errorCode = 1;
        break;
    case CmdType::EXTRACT_ALL_PNG:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (output.length() == 0)
        {
            PERROR("Output param empty!\n");
            errorCode = 1;
            break;
        }
        if (!tools.extractAllTextures(gameId, output, true, pccOnly, tfcOnly, tfcName))
            errorCode = 1;
        break;
    case CmdType::DLC_MOD_TEXTURES:
    {
        if (gameId == MeType::UNKNOWN_TYPE || gameId == MeType::ME1_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        if (tfcName.length() == 0)
        {
            PERROR("TFC name param missing!\n");
            errorCode = 1;
            break;
        }
        if (!input.endsWith(".mem", Qt::CaseInsensitive))
        {
            PERROR("Input file is not mem!\n");
            errorCode = 1;
            break;
        }
        if (!QFile(input).exists())
        {
            PERROR("Input file doesn't exists! " + input + "\n");
            errorCode = 1;
            break;
        }
        quint8 guidArray[16];
        memset(guidArray, 0, 16);
        QByteArray array;
        if (guid.length() != 0)
        {
            for (int i = 0; i < 32; i += 2)
            {
                bool ok;
                guidArray[i / 2] = guid.midRef(i, 2).toInt(&ok, 16);
                if (!ok)
                {
                    PERROR("Guid param is wrong!\n");
                    errorCode = 1;
                    break;
                }
            }
            array = QByteArray(reinterpret_cast<char *>(guidArray), 16);
        }
        if (!tools.applyMEMSpecialModME3(gameId, input, tfcName, array, verify))
            errorCode = 1;
        break;
    }
    case CmdType::UNPACK_ARCHIVE:
        if (!QFile(input).exists())
        {
            PERROR("Input file doesn't exists!\n");
            errorCode = 1;
            break;
        }
        if (!tools.unpackArchive(input, output))
            errorCode = 1;
        break;
#if !defined(_WIN32)
    case CmdType::SET_GAME_DATA_PATH:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        path = QDir::cleanPath(path);
        if (path.length() != 0 && !QDir(path).exists())
        {
            PERROR("Game path doesn't exists!\n");
            errorCode = 1;
            break;
        }
        if (!Misc::SetGameDataPath(gameId, path))
            errorCode = 1;
        break;
    case CmdType::SET_GAME_USER_PATH:
        if (gameId == MeType::UNKNOWN_TYPE)
        {
            PERROR("Wrong game id!\n");
            errorCode = 1;
            break;
        }
        path = QDir::cleanPath(path);
        if (path.length() != 0 && !QDir(path).exists())
        {
            PERROR("Game config path doesn't exists!\n");
            errorCode = 1;
            break;
        }
        if (!Misc::SetGameUserPath(gameId, path))
            errorCode = 1;
        break;
#endif
    }

    return errorCode;
}
