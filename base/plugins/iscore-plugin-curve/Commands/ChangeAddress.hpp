#pragma once
#include <DeviceExplorer/Address/AddressSettings.hpp>

#include <iscore/tools/ModelPath.hpp>
#include <iscore/command/SerializableCommand.hpp>

class AutomationModel;
class ChangeAddress : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL_OBSOLETE("ChangeAddress", "ChangeAddress")
    public:
        ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR_OBSOLETE(ChangeAddress, "AutomationControl")
        ChangeAddress(
                Path<AutomationModel>&& path,
                const iscore::Address& newval);

    public:
        void undo();
        void redo();

    protected:
        void serializeImpl(QDataStream &) const;
        void deserializeImpl(QDataStream &);

    private:
        Path<AutomationModel> m_path;
        iscore::FullAddressSettings m_old, m_new;
};

