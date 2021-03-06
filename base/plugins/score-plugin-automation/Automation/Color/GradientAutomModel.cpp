// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Automation/Color/GradientAutomModel.hpp>
#include <Automation/Color/GradientAutomPresenter.hpp>
#include <ossia/editor/state/destination_qualifiers.hpp>
#include <QColor>
namespace Gradient
{
ProcessModel::ProcessModel(
    const TimeVal& duration,
    const Id<Process::ProcessModel>& id,
    QObject* parent)
    : Process::ProcessModel{duration, id,
                        Metadata<ObjectKey_k, ProcessModel>::get(), parent}
{
  m_colors.insert(std::make_pair(0.2, QColor(Qt::black)));
  m_colors.insert(std::make_pair(0.8, QColor(Qt::white)));

  metadata().setInstanceName(*this);
}

ProcessModel::~ProcessModel()
{
}

ProcessModel::ProcessModel(
    const ProcessModel& source,
    const Id<Process::ProcessModel>& id,
    QObject* parent)
    : Process::ProcessModel{
        source, id, Metadata<ObjectKey_k, ProcessModel>::get(), parent}
    , m_address(source.m_address)
    , m_colors{source.m_colors}
    , m_tween{source.m_tween}
{
}

QString ProcessModel::prettyName() const
{
  return address().toString();
}

void ProcessModel::setDurationAndScale(const TimeVal& newDuration)
{
  // We only need to change the duration.
  setDuration(newDuration);
}

void ProcessModel::setDurationAndGrow(const TimeVal& newDuration)
{
  setDuration(newDuration);
}

void ProcessModel::setDurationAndShrink(const TimeVal& newDuration)
{
  setDuration(newDuration);
}

bool ProcessModel::contentHasDuration() const
{
  return true;
}

TimeVal ProcessModel::contentDuration() const
{
  auto lastPoint = 1.;
  if(!m_colors.empty())
  {
    auto back = m_colors.rbegin()->first;
    lastPoint = std::max(1., back);
  }

  return duration() * lastPoint;
}

::State::AddressAccessor ProcessModel::address() const
{
  return m_address;
}

void ProcessModel::setAddress(const ::State::AddressAccessor& arg)
{
  if (m_address == arg)
  {
    return;
  }

  m_address = arg;
  emit addressChanged(arg);
}

}


template <>
void DataStreamReader::read(
    const Gradient::ProcessModel& autom)
{
  m_stream << autom.m_address
           << autom.m_colors
           << autom.m_tween;
  insertDelimiter();
}


template <>
void DataStreamWriter::write(Gradient::ProcessModel& autom)
{
  m_stream >> autom.m_address
           >> autom.m_colors
           >> autom.m_tween;

  checkDelimiter();
}


template <>
void JSONObjectReader::read(
    const Gradient::ProcessModel& autom)
{
  obj[strings.Address] = toJsonObject(autom.address());
  JSONValueReader v{}; v.readFrom(autom.m_colors);
  obj["Gradient"] = v.val;
  obj["Tween"] = autom.tween();
}


template <>
void JSONObjectWriter::write(Gradient::ProcessModel& autom)
{
  autom.setAddress(
      fromJsonObject<State::AddressAccessor>(obj[strings.Address]));
  autom.setTween(obj["Tween"].toBool());
  JSONValueWriter v{}; v.val = obj["Gradient"];
  v.writeTo(autom.m_colors);
}
