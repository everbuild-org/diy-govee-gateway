package org.everbuild.govee.plugins

import io.ktor.server.application.*
import io.ktor.server.request.*
import io.ktor.server.response.*
import io.ktor.server.routing.*
import io.micrometer.core.instrument.Gauge
import kotlinx.serialization.Serializable

@Serializable
data class Payload(val id: String, val temperature: Float, val humidity: Float, val battery: Float)

data class GaugeSet(
    val id: String,
    var temperature: Float,
    var humidity: Float,
    var battery: Float
) {
    constructor(payload: Payload) : this(
        payload.id,
        payload.temperature,
        payload.humidity,
        payload.battery
    ) {
        Gauge.builder("${payload.id}_temperature") {
            println("TEMP REQUEST")
            this.temperature
        }.register(appMicrometerRegistry)
        Gauge.builder("${payload.id}_humidity") { this.humidity }.register(appMicrometerRegistry)
        Gauge.builder("${payload.id}_battery") { this.battery }.register(appMicrometerRegistry)
    }
}

val gauges = hashMapOf<String, GaugeSet>()

fun Application.configureRouting() {
    routing {
        put("/api") {
            val payload = call.receive<Payload>()
            val idGauges = gauges.getOrPut(payload.id) { GaugeSet(payload) }
            idGauges.temperature = payload.temperature
            idGauges.humidity = payload.humidity
            idGauges.battery = payload.battery

            call.respondText("OK")
        }
    }
}
