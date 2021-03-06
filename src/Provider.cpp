#include "Provider.hpp"
#include "Stream.hpp"
#include "Executable.hpp"
#include "LambdaExecutable.hpp"
#include <QObject>
#include <QMetaObject>

quickstreams::Provider::Provider(QObject* parent) :
	QObject(parent),
	_totalCreated(0),
	_totalExisting(0),
	_totalActive(0)
{}

quickstreams::Stream::Reference quickstreams::Provider::internalCreate(
	const Executable::Reference& executable,
	quickstreams::Stream::Type type
) {
	auto stream(new Stream(
		this,
		executable,
		type,
		Stream::CaptionStatus::Free
	));
	Stream::Reference reference(stream, &Stream::deleteLater);
	registerNew(reference);

	QMetaObject::invokeMethod(
		stream, "initialize",
		Qt::QueuedConnection
	);

	return reference;
}

void quickstreams::Provider::registerNew(const Stream::Reference& reference) {
	_references.insert(reference.data(), reference);

	// Update statistics
	++_totalCreated;
	++_totalExisting;
	totalCreatedChanged();
	totalExistingChanged();
}

void quickstreams::Provider::activated() {
	// Update statistics
	++_totalActive;
	totalActiveChanged();
}

void quickstreams::Provider::finished() {
	// Update statistics
	--_totalActive;
	totalActiveChanged();
}

void quickstreams::Provider::destroyed() {
	// Update statistics
	--_totalExisting;
	totalExistingChanged();
}

void quickstreams::Provider::dispose(Stream* stream) {
	_references.erase(_references.find(stream));
}

quickstreams::Stream::Reference quickstreams::Provider::reference(
	Stream* stream
) const {
	return _references.constFind(stream).value();
}

quickstreams::Stream::Reference quickstreams::Provider::create(
	LambdaExecutable::Function function,
	quickstreams::Stream::Type type
) {
	return internalCreate(
		Executable::Reference(new LambdaExecutable(function)),
		type
	);
}

quint64 quickstreams::Provider::totalCreated() const {
	return _totalCreated;
}

quint64 quickstreams::Provider::totalExisting() const {
	return _totalExisting;
}

quint64 quickstreams::Provider::totalActive() const {
	return _totalActive;
}
