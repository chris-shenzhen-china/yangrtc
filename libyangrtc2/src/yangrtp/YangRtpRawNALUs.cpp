
#include <yangrtp/YangRtpRawNALUs.h>
#include <yangutil/sys/YangLog.h>
YangRtpRawNALUs::YangRtpRawNALUs() {
	m_cursor = 0;
	m_nn_bytes = 0;
}

YangRtpRawNALUs::~YangRtpRawNALUs() {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		yang_delete(p);
	}
}

void YangRtpRawNALUs::push_back(YangSample *sample) {
	if (sample->m_size <= 0) {
		return;
	}

	if (!m_nalus.empty()) {
		YangSample *p = new YangSample();
		p->m_bytes = (char*) "\0\0\1";
		p->m_size = 3;
		m_nn_bytes += 3;
		m_nalus.push_back(p);
	}

	m_nn_bytes += sample->m_size;
	m_nalus.push_back(sample);
}

uint8_t YangRtpRawNALUs::skip_first_byte() {
	// srs_assert (cursor >= 0 && nn_bytes > 0 && cursor < nn_bytes);
	m_cursor++;
	return uint8_t(m_nalus[0]->m_bytes[0]);
}

int32_t YangRtpRawNALUs::read_samples(vector<YangSample*> &samples, int32_t packet_size) {
	if (m_cursor + packet_size < 0 || m_cursor + packet_size > m_nn_bytes) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER,
				"cursor=%d, max=%d, size=%d", m_cursor, m_nn_bytes, packet_size);
	}

	int32_t pos = m_cursor;
	m_cursor += packet_size;
	int32_t left = packet_size;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; left > 0 && i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];

		// Ignore previous consumed samples.
		if (pos && pos - p->m_size >= 0) {
			pos -= p->m_size;
			continue;
		}

		// Now, we are working at the sample.
		int32_t nn = yang_min(left, p->m_size - pos);
		// srs_assert(nn > 0);

		YangSample *sample = new YangSample();
		samples.push_back(sample);

		sample->m_bytes = p->m_bytes + pos;
		sample->m_size = nn;

		left -= nn;
		pos = 0;
	}

	return Yang_Ok;
}

uint64_t YangRtpRawNALUs::nb_bytes() {
	int32_t size = 0;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		size += p->m_size;
	}

	return size;
}

int32_t YangRtpRawNALUs::encode(YangBuffer *buf) {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];

		if (!buf->require(p->m_size)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					p->m_size);
		}

		buf->write_bytes(p->m_bytes, p->m_size);
	}

	return Yang_Ok;
}

int32_t YangRtpRawNALUs::decode(YangBuffer *buf) {
	if (buf->empty()) {
		return Yang_Ok;
	}

	YangSample *sample = new YangSample();
	sample->m_bytes = buf->head();
	sample->m_size = buf->left();
	buf->skip(sample->m_size);

	m_nalus.push_back(sample);

	return Yang_Ok;
}

IYangRtpPayloader* YangRtpRawNALUs::copy() {
	YangRtpRawNALUs *cp = new YangRtpRawNALUs();

	cp->m_nn_bytes = m_nn_bytes;
	cp->m_cursor = m_cursor;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		cp->m_nalus.push_back(p->copy());
	}

	return cp;
}
