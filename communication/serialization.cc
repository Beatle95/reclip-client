#include "communication/serialization.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <cassert>

constexpr char kHostIdKey[] = "ClientId";
constexpr char kHostNameKey[] = "ClientName";

namespace reclip {

namespace {

std::optional<HostData> ParseHost(const QJsonObject& obj) {
  const auto id = obj.find(kHostIdKey);
  if (id == obj.end() || !id->isString()) {
    return {};
  }
  const auto name = obj.find(kHostNameKey);
  if (name == obj.end() || !name->isString()) {
    return {};
  }

  const auto text_data = obj.find("TextData");
  if (text_data == obj.end() || !text_data->isArray()) {
    return {};
  }
  std::deque<std::string> text;
  auto text_arr = text_data->toArray();
  for (auto entry : text_arr) {
    if (!entry.isString()) {
      return {};
    }
    text.push_back(entry.toString().toStdString());
  }

  return HostData{
      .id = id->toString().toStdString(),
      .name = name->toString().toStdString(),
      .data = {.text = std::move(text)},
  };
}

SerializationTestHelper* g_test_helper = nullptr;

}  // namespace

QByteArray SerializeHostSyncRequest(const HostId& id) {
  QJsonObject root;
  root[kHostIdKey] = QString::fromStdString(id);
  return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

QByteArray SerializeTextUpdate(const std::string& text) {
  QJsonObject root;
  root["Text"] = QString::fromStdString(text);
  return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

QByteArray SerializeHostSync(const HostData& data) {
  QJsonArray text;
  for (const auto& entry : data.data.text) {
    text.push_back(QString::fromStdString(entry));
  }

  QJsonObject root;
  root[kHostIdKey] = QString::fromStdString(data.id);
  root[kHostNameKey] = QString::fromStdString(data.name);
  root["TextData"] = text;

  return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

std::optional<SyncResponse> ParseSyncResponse(const QByteArray& data) {
  if (g_test_helper) {
    return g_test_helper->ParseSyncResponse(data);
  }

  auto doc = QJsonDocument::fromJson(data);
  if (doc.isEmpty()) {
    return {};
  }

  auto root = doc.object();
  if (root.isEmpty()) {
    return {};
  }

  auto this_host_it = root.find("ThisHostData");
  auto hosts_data_it = root.find("OtherData");
  if (this_host_it == root.end() || hosts_data_it == root.end() ||
      !this_host_it->isObject() || !hosts_data_it->isArray()) {
    return {};
  }

  auto this_host = ParseHost(this_host_it->toObject());
  if (!this_host) {
    return {};
  }
  std::vector<HostData> hosts;
  for (auto obj : hosts_data_it->toArray()) {
    if (!obj.isObject()) {
      return {};
    }
    auto host_obj = ParseHost(obj.toObject());
    if (!host_obj) {
      return {};
    }
    hosts.push_back(std::move(*host_obj));
  }

  return SyncResponse{std::move(*this_host), std::move(hosts)};
}

std::optional<HostId> ParseHostId(const QByteArray& data) {
  if (g_test_helper) {
    return g_test_helper->ParseHostId(data);
  }

  auto doc = QJsonDocument::fromJson(data);
  if (doc.isEmpty()) {
    return {};
  }
  auto root = doc.object();
  if (root.isEmpty()) {
    return {};
  }

  const auto id = root.find(kHostIdKey);
  if (id == root.end() || !id->isString()) {
    return {};
  }
  return id->toString().toStdString();
}

std::optional<HostData> ParseHostData(const QByteArray& data) {
  if (g_test_helper) {
    return g_test_helper->ParseHostData(data);
  }
  auto doc = QJsonDocument::fromJson(data);
  if (doc.isEmpty()) {
    return {};
  }
  return ParseHost(doc.object());
}

std::optional<TextUpdateResponse> ParseNewText(const QByteArray& data) {
  if (g_test_helper) {
    return g_test_helper->ParseNewText(data);
  }

  auto doc = QJsonDocument::fromJson(data);
  if (doc.isEmpty()) {
    return {};
  }
  auto root = doc.object();
  if (root.isEmpty()) {
    return {};
  }

  const auto id = root.find(kHostIdKey);
  if (id == root.end() || !id->isString()) {
    return {};
  }
  const auto text = root.find("Text");
  if (text == root.end() || !text->isString()) {
    return {};
  }
  return TextUpdateResponse{.id = id->toString().toStdString(),
                            .text = text->toString().toStdString()};
}

SerializationTestHelper::SerializationTestHelper() {
  assert(g_test_helper == nullptr);
  g_test_helper = this;
}

SerializationTestHelper::~SerializationTestHelper() {
  assert(g_test_helper == this);
  g_test_helper = nullptr;
}

}  // namespace reclip
